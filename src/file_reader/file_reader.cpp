#include "file_reader.hpp"
#include <fcntl.h> // ::open
#include <sys/mman.h> // ::mmap, ::munmap
#include <unistd.h> // ::lseek
#include <cerrno>
#include <cstring> // std::strerror
#include <iostream>
#include <tuple>


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

file_reader::file_reader(std::filesystem::path const& input_file)
        : input_file_(input_file)
        , f_ptr_(nullptr)
        , file_size_(0)
        , stats_()
{
    std::tie(f_ptr_, file_size_) = mmap_file(input_file_);
    if (f_ptr_ == nullptr)
        throw std::runtime_error(fmt::format("failed to mmap file: {}", input_file_.c_str()));
}

file_reader::~file_reader() noexcept
{
    if (::munmap(f_ptr_, file_size_) == -1)
        fmt::print(stderr, "ERROR: munmap [{}]\n", std::strerror(errno));
}

void
file_reader::print_stats() const noexcept
{
    fmt::print("reader stats\n");
    fmt::print("  file size:       {}\n", file_size_);
    fmt::print("  bytes processed: {}\n", stats_.byte_count);
    fmt::print("  shift count:     {}\n", stats_.shift_count);
    fmt::print("  bytes shifted:   {}\n", stats_.bytes_shifted);
    fmt::print("  inflate count:   {}\n", stats_.inflate_count);
    fmt::print("  time:            {}\n", to_time_str(stats_.nsec_count));
}
