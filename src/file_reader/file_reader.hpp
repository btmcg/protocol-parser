#pragma once

#include "util/byte_buffer.hpp"
#include "util/time.hpp"
#include <filesystem>
#include <fmt/format.h>
#include <zlib.h>
#include <cstddef>
#include <cstdint>


class file_reader
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
        std::uint64_t nsec_count = 0;
    };

private:
    std::filesystem::path input_file_;
    void* f_ptr_;
    off_t file_size_;
    stats stats_;

public:
    file_reader(std::filesystem::path const&);
    ~file_reader() noexcept;
    file_reader(file_reader const&) noexcept = delete;
    file_reader(file_reader&&) noexcept = delete;
    file_reader& operator=(file_reader const&) noexcept = delete;
    file_reader& operator=(file_reader&&) noexcept = delete;

    void print_stats() const;

    template <typename Callable>
    bool process_file(Callable&& fn);

private:
    template <typename Callable>
    bool process_raw(Callable&& fn);

    template <typename Callable>
    bool process_gz(Callable&& fn);
};

/**********************************************************************/

template <typename Callable>
bool
file_reader::process_file(Callable&& fn)
{
    return (input_file_.extension() == ".gz") ? process_gz(fn) : process_raw(fn);
}

template <typename Callable>
bool
file_reader::process_raw(Callable&& fn)
{
    auto ptr = reinterpret_cast<std::uint8_t const*>(f_ptr_);
    stats_.nsec_count += tsc::get_nsecs();
    std::size_t const bytes_processed = fn(ptr, file_size_);
    stats_.nsec_count = tsc::get_nsecs() - stats_.nsec_count;
    stats_.byte_count += bytes_processed;
    return true;
}

template <typename Callable>
bool
file_reader::process_gz(Callable&& fn)
{
    stats_.nsec_count += tsc::get_nsecs();

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
                fmt::print(stderr, "Z_STREAM_ERROR\n");
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

        std::size_t const bytes_processed = fn(buf.read_ptr(), buf.bytes_unread());
        buf.bytes_read(bytes_processed);

        ++stats_.shift_count;
        ++stats_.bytes_shifted += buf.shift();
        stats_.byte_count += bytes_processed;

    } while (ret != Z_STREAM_END);

    ::inflateEnd(&zstrm);

    stats_.nsec_count = tsc::get_nsecs() - stats_.nsec_count;
    return true;
}
