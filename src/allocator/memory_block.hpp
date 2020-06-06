#pragma once

#include <cstddef> // std::size_t
#include <cstdint>


struct memory_block
{
    void* memory = nullptr;
    std::size_t size = 0;

    constexpr memory_block() noexcept = default;
    constexpr memory_block(void* mem, std::size_t s) noexcept
            : memory(mem)
            , size(s)
    {
        // empty
    }

    constexpr bool
    contains(void const* address) const noexcept
    {
        if (address == nullptr)
            return false;

        std::uint8_t const* mem = static_cast<std::uint8_t const*>(memory);
        std::uint8_t const* addr = static_cast<std::uint8_t const*>(address);
        return addr >= mem && addr < mem + size;
    }
};
