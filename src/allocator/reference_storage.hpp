#pragma once

#include "memory_pool.hpp"


class reference_storage
{
public:
    using allocator_type = memory_pool;

    reference_storage() noexcept = default;
    ~reference_storage() noexcept = default;

    reference_storage(memory_pool& alloc) noexcept
            : alloc_(&alloc)
    {
        // empty
    }

    reference_storage(reference_storage const&) noexcept = default;
    reference_storage& operator=(reference_storage const&) noexcept = default;

    bool
    is_valid() const noexcept
    {
        return alloc_ != nullptr;
    }

    memory_pool&
    get_allocator() const noexcept
    {
        return *alloc_;
    }

private:
    memory_pool* alloc_ = nullptr;
};
