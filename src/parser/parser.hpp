#pragma once

#include "byte_buffer.hpp"
#include <filesystem>
#include <cstddef>
#include <cstdint>

class parser
{
private:
    enum
    {
        BufferSize = 1024 * 1024
    };

    struct stats
    {
        std::size_t byte_count = 0;
        std::size_t msg_count = 0;
    };

private:
    std::filesystem::path const& input_file_;
    void* f_ptr_;
    off_t file_size_;
    stats stats_;

public:
    parser(std::filesystem::path const&);
    ~parser() noexcept;
    bool parse() noexcept;
    void print_stats() const noexcept;

private:
    void parse_itch(byte_buffer<BufferSize>&) noexcept;
};
