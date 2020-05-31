#pragma once

#include "memory_block.hpp"
#include "memory_block_stack.hpp"
#include "util/assert.hpp"
#include <cstdint>
#include <utility>


template <typename BlockAllocator>
class memory_arena : private BlockAllocator
{
private:
    memory_block_stack used_;

public:
    constexpr explicit memory_arena(std::size_t block_size);

    /// Deallocates all memory blocks that where requested back to the
    /// BlockAllocator.
    constexpr ~memory_arena() noexcept;

    memory_arena(memory_arena&& other) noexcept;
    memory_arena& operator=(memory_arena&& other) noexcept;

    memory_block allocate_block();
    constexpr memory_block current_block() const noexcept;
    void deallocate_block() noexcept;
    constexpr bool owns(void const* ptr) const noexcept;
    constexpr std::size_t size() const noexcept;
    constexpr std::size_t next_block_size() const noexcept;
    BlockAllocator& get_allocator() noexcept;

    friend void
    swap(memory_arena& a, memory_arena& b) noexcept
    {
        std::swap(static_cast<BlockAllocator&>(a), static_cast<BlockAllocator&>(b));
        std::swap(a.used_, b.used_);
    }
};

/**********************************************************************/

template <typename BlockAllocator>
constexpr memory_arena<BlockAllocator>::memory_arena(std::size_t block_size)
        : BlockAllocator(block_size)
{
    // empty
}

template <typename BlockAllocator>
constexpr memory_arena<BlockAllocator>::~memory_arena() noexcept
{
    while (!used_.empty())
        BlockAllocator::deallocate_block(used_.pop());
}

template <typename BlockAllocator>
memory_arena<BlockAllocator>::memory_arena(memory_arena&& other) noexcept
        : BlockAllocator(std::move(other))
        , used_(std::move(other.used_))
{
    // empty
}

template <typename BlockAllocator>
memory_arena<BlockAllocator>&
memory_arena<BlockAllocator>::operator=(memory_arena&& other) noexcept
{
    memory_arena tmp(std::move(other));
    swap(*this, tmp);
    return *this;
}

template <typename BlockAllocator>
memory_block
memory_arena<BlockAllocator>::allocate_block()
{
    used_.push(BlockAllocator::allocate_block());
    return used_.top();
}

template <typename BlockAllocator>
constexpr memory_block
memory_arena<BlockAllocator>::current_block() const noexcept
{
    return used_.top();
}

template <typename BlockAllocator>
void
memory_arena<BlockAllocator>::deallocate_block() noexcept
{
    BlockAllocator::deallocate_block(used_.pop());
}

template <typename BlockAllocator>
constexpr bool
memory_arena<BlockAllocator>::owns(const void* ptr) const noexcept
{
    return used_.owns(ptr);
}

template <typename BlockAllocator>
constexpr std::size_t
memory_arena<BlockAllocator>::size() const noexcept
{
    return used_.size();
}

template <typename BlockAllocator>
constexpr std::size_t
memory_arena<BlockAllocator>::next_block_size() const noexcept
{
    return BlockAllocator::next_block_size() - memory_block_stack::implementation_offset;
}

template <typename BlockAllocator>
BlockAllocator&
memory_arena<BlockAllocator>::get_allocator() noexcept
{
    return *this;
}
