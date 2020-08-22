#pragma once

#include "lowlevel_allocator.hpp"
#include "memory_block.hpp"
#include <cstddef> // std::size_t
#include <cstdint>


template <typename LLAllocator = lowlevel_allocator<malloc_allocator>, std::uint16_t Num = 2,
        std::uint16_t Den = 1>
class growing_block_allocator : private LLAllocator
{
    static_assert(static_cast<float>(Num) / Den >= 1.0, "invalid growth factor");

private:
    static constexpr std::size_t DefaultAlignment = 8;

private:
    std::size_t block_size_ = 0;

public:
    constexpr explicit growing_block_allocator(
            std::size_t block_size, LLAllocator alloc = LLAllocator()) noexcept;
    memory_block allocate_block() noexcept;
    constexpr void deallocate_block(memory_block block) const noexcept;
    constexpr std::size_t next_block_size() const noexcept;
    constexpr float growth_factor() const noexcept;
};

/**********************************************************************/

template <typename LLAllocator, std::uint16_t Num, std::uint16_t Den>
constexpr growing_block_allocator<LLAllocator, Num, Den>::growing_block_allocator(
        std::size_t block_size, LLAllocator alloc) noexcept
        : LLAllocator(std::move(alloc))
        , block_size_(block_size)
{
    // empty
}

template <typename LLAllocator, std::uint16_t Num, std::uint16_t Den>
memory_block
growing_block_allocator<LLAllocator, Num, Den>::allocate_block() noexcept
{
    void* memory = LLAllocator::allocate_node(block_size_, DefaultAlignment);
    memory_block block(memory, block_size_);
    block_size_ = static_cast<std::size_t>(block_size_ * growth_factor());
    return block;
}

template <typename LLAllocator, std::uint16_t Num, std::uint16_t Den>
constexpr void
growing_block_allocator<LLAllocator, Num, Den>::deallocate_block(memory_block block) const noexcept
{
    LLAllocator::deallocate_node(block.memory, block.size, DefaultAlignment);
}

template <typename LLAllocator, std::uint16_t Num, std::uint16_t Den>
constexpr std::size_t
growing_block_allocator<LLAllocator, Num, Den>::next_block_size() const noexcept
{
    return block_size_;
}

template <typename LLAllocator, std::uint16_t Num, std::uint16_t Den>
constexpr float
growing_block_allocator<LLAllocator, Num, Den>::growth_factor() const noexcept
{
    return static_cast<float>(Num) / Den;
}
