#pragma once

#include "common/byte_buffer.hpp"
#include <filesystem>
#include <cstddef>
#include <cstdint>
#include <unordered_map>

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
    std::size_t parse_itch(std::uint8_t const*, std::size_t) noexcept;
};
