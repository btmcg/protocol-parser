#pragma once

#include "util/assert.hpp"
#include <cstddef> // std::size_t
#include <cstdint>


namespace detail {

    constexpr int DefaultAlignment = 8;
    constexpr std::size_t MinElementSize = sizeof(std::uint8_t*);


    /// returns next greater multiple of 8 to ensure 8-byte alignment
    constexpr std::size_t
    round_up_to_align(std::size_t& x, int alignment = DefaultAlignment)
    {
        return ((x + (alignment - 1)) & (-alignment));
    }

    /// whether or not an alignment is valid, i.e. a power of two not zero
    constexpr bool
    is_valid_alignment(std::size_t alignment) noexcept
    {
        return alignment && (alignment & (alignment - 1)) == 0u;
    }

    inline bool
    is_aligned(void* ptr, std::size_t alignment) noexcept
    {
        DEBUG_ASSERT(is_valid_alignment(alignment));
        auto address = reinterpret_cast<std::uintptr_t>(ptr);
        return address % alignment == 0u;
    }

} // namespace detail
