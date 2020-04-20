#pragma once

#include <filesystem>
#include <cstdint>

class parser
{
public:
    parser(std::filesystem::path const&);
    ~parser() noexcept;
    bool parse() noexcept;

private:
    std::uint16_t parse_itch(std::uint8_t const*) noexcept;

private:
    enum
    {
        BufferSize = 1024 * 1024
    };

private:
    std::filesystem::path const& input_file_;
    void* f_ptr_;
    off_t file_size_;
};
