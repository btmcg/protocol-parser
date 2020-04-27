#pragma once

#include "common/byte_buffer.hpp"
#include <endian.h>
#include <fcntl.h> // ::open
#include <filesystem>
#include <fmt/format.h>
#include <sys/mman.h> // ::mmap, ::munmap
#include <unistd.h> // ::lseek
#include <zlib.h>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstring> // std::strerror
#include <iostream>
#include <tuple>
#include <unordered_map>

namespace { // unnamed

    std::tuple<void*, off_t>
    mmap_file(std::filesystem::path const& filepath) noexcept
    {
        int const fd = ::open(filepath.c_str(), O_RDONLY);
        if (fd == -1) {
            fmt::print(stderr, "ERROR: open ({}): [{}]\n", filepath.c_str(), std::strerror(errno));
            return {nullptr, 0};
        }

        off_t const file_size = ::lseek(fd, 0, SEEK_END);
        if (file_size == -1) {
            fmt::print(stderr, "ERROR: lseek [{}]\n", std::strerror(errno));
            ::close(fd);
            return {nullptr, 0};
        }

        void* f_ptr = ::mmap(nullptr, file_size, PROT_READ, MAP_PRIVATE, fd, 0);
        if (f_ptr == MAP_FAILED) {
            fmt::print(stderr, "ERROR: mmap [{}]\n", std::strerror(errno));
            ::close(fd);
            return {nullptr, 0};
        }

        if (::close(fd) == -1) {
            // not a critical failure
            fmt::print(stderr, "ERROR: close [{}]\n", std::strerror(errno));
        }

        return std::make_tuple(f_ptr, file_size);
    }

} // namespace

template <typename Protocol>
class file_parser
{
private:
    enum
    {
        BufferSize = 10 * 1024 * 1024
    };

    struct stats
    {
        std::size_t shift_count = 0;
        std::size_t bytes_shifted = 0;
        std::size_t inflate_count = 0;
        std::size_t byte_count = 0;
        std::size_t msg_count = 0;
        std::unordered_map<char, std::size_t> msg_type_count;
    };

private:
    std::filesystem::path const& input_file_;
    void* f_ptr_;
    off_t file_size_;
    stats stats_;

public:
    file_parser(std::filesystem::path const&);
    ~file_parser() noexcept;
    bool parse() noexcept;
    void print_stats() const noexcept;

private:
    bool parse_raw() noexcept;
    bool parse_gz() noexcept;
};

/**********************************************************************/

template <typename Protocol>
file_parser<Protocol>::file_parser(std::filesystem::path const& input_file)
        : input_file_(input_file)
        , f_ptr_(nullptr)
        , file_size_(0)
        , stats_()
{
    std::tie(f_ptr_, file_size_) = mmap_file(input_file_);
    if (f_ptr_ == nullptr)
        throw std::runtime_error(fmt::format("failed to mmap file: {}", input_file_.c_str()));
}

template <typename Protocol>
file_parser<Protocol>::~file_parser() noexcept
{
    if (::munmap(f_ptr_, file_size_) == -1)
        fmt::print(stderr, "ERROR: munmap [{}]\n", std::strerror(errno));
}

template <typename Protocol>
bool
file_parser<Protocol>::parse() noexcept
{
    return input_file_.extension() == ".gz" ? parse_gz() : parse_raw();
}

template <typename Protocol>
bool
file_parser<Protocol>::parse_raw() noexcept
{
    fmt::print("file_size_={}\n", file_size_);

    std::uint8_t const* ptr = reinterpret_cast<decltype(ptr)>(f_ptr_);

    static_cast<Protocol&>(*this).parse(ptr, file_size_);
    return true;
}

template <typename Protocol>
bool
file_parser<Protocol>::parse_gz() noexcept
{
    fmt::print("file_size_={}\n", file_size_);

    ::z_stream zstrm;
    zstrm.zalloc = nullptr;
    zstrm.zfree = nullptr;
    zstrm.opaque = nullptr;
    zstrm.avail_in = 0;
    zstrm.next_in = nullptr;

    if (int const rv = ::inflateInit2(&zstrm, 32); rv != Z_OK) {
        fmt::print(stderr, "ERROR: inflateInit returned error {}\n", rv);
        return false;
    }

    byte_buffer<BufferSize> buf;
    std::size_t total_bytes_inflated = 0;

    zstrm.avail_in = file_size_;
    zstrm.next_in = reinterpret_cast<decltype(zstrm.next_in)>(f_ptr_);

    int ret = 0;
    do {
        zstrm.avail_out = buf.bytes_left();
        zstrm.next_out = buf.write_ptr();

        ++stats_.inflate_count;
        ret = ::inflate(&zstrm, Z_NO_FLUSH);
        switch (ret) {
            case Z_STREAM_ERROR:
                fmt::print(stderr, "Z_NEED_DICT\n");
                return false;
            case Z_NEED_DICT:
                fmt::print(stderr, "Z_NEED_DICT\n");
                return false;
            case Z_DATA_ERROR:
                fmt::print(stderr, "Z_DATA_ERROR\n");
                return false;
            case Z_MEM_ERROR:
                fmt::print(stderr, "Z_MEM_ERROR\n");
                (void)inflateEnd(&zstrm);
                return false;
        }
        buf.bytes_written(zstrm.total_out - total_bytes_inflated);
        total_bytes_inflated = zstrm.total_out;

        std::size_t const bytes_processed = static_cast<Protocol&>(*this).parse(buf.read_ptr(), buf.bytes_unread());
        buf.bytes_read(bytes_processed);

        ++stats_.shift_count;
        ++stats_.bytes_shifted += buf.shift();

    } while (ret != Z_STREAM_END);

    return true;
}

template <typename Protocol>
void
file_parser<Protocol>::print_stats() const noexcept
{
    fmt::print(
            "bytes shifted:    {}\ncalls to shift:   {}\ncalls to inflate: {}\nbytes_processed:  {}\nmsgs_processed:   {}\n",
            stats_.bytes_shifted, stats_.shift_count, stats_.inflate_count, stats_.byte_count,
            stats_.msg_count);
    for (auto const& itr : stats_.msg_type_count) {
        fmt::print("    count={}, msg_type={}\n", itr.second, itr.first);
    }
}
