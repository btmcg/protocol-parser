#include "file_reader.hpp"
#include <fcntl.h> // ::open
#include <sys/mman.h> // ::mmap, ::munmap
#include <unistd.h> // ::lseek
#include <cerrno>
#include <cstring> // std::strerror
#include <stdexcept> // std::runtime_error
#include <tuple>


namespace { // unnamed

    std::tuple<void*, off_t>
    mmap_file(std::filesystem::path const& filepath)
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
        if (f_ptr == MAP_FAILED) { // NOLINT
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
    try {
        if (::munmap(f_ptr_, file_size_) == -1)
            fmt::print(stderr, "ERROR: munmap [{}]\n", std::strerror(errno));
    } catch (...) {
        // suppress
    }
}

void
file_reader::print_stats() const
{
    // clang-format off
    fmt::print("reader stats\n"
               "  file size:       {}\n"
               "  bytes processed: {}\n"
               "  shift count:     {}\n"
               "  bytes shifted:   {}\n"
               "  inflate count:   {}\n"
               "  time:            {}\n",
        file_size_,
        stats_.byte_count,
        stats_.shift_count,
        stats_.bytes_shifted,
        stats_.inflate_count,
        to_time_str(stats_.nsec_count));
    // clang-format on
}
