#pragma once

#include "memory_pool.hpp"


class reference_storage
{
public:
    using allocator_type = memory_pool;

    constexpr reference_storage() noexcept = default;
    constexpr ~reference_storage() noexcept = default;

    constexpr reference_storage(memory_pool& alloc) noexcept
            : alloc_(&alloc)
    {
        // empty
    }

    constexpr reference_storage(reference_storage const&) noexcept = default;
    constexpr reference_storage& operator=(reference_storage const&) noexcept = default;

    constexpr bool
    is_valid() const noexcept
    {
        return alloc_ != nullptr;
    }

    constexpr memory_pool&
    get_allocator() const noexcept
    {
        return *alloc_;
    }

private:
    memory_pool* alloc_ = nullptr;
};
