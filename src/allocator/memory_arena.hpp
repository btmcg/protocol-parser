#pragma once

#include "lowlevel_allocator.hpp"
#include "util/assert.hpp"
#include <cstdint>
#include <utility>


struct memory_block
{
    void* memory = nullptr;
    std::size_t size = 0;

    memory_block() noexcept;
    memory_block(void* mem, std::size_t s) noexcept;
    memory_block(void* begin, void* end) noexcept;

    bool contains(const void* address) const noexcept;
};

/**********************************************************************/

template <typename LLAllocator = lowlevel_allocator<malloc_allocator>, std::uint16_t Num = 2,
        std::uint16_t Den = 1>
class growing_block_allocator : LLAllocator
{
    static_assert(float(Num) / Den >= 1.0, "invalid growth factor");

public:
    explicit growing_block_allocator(
            std::size_t block_size, LLAllocator alloc = LLAllocator()) noexcept
            : LLAllocator(std::move(alloc))
            , block_size_(block_size)
    {
        // empty
    }

    memory_block
    allocate_block()
    {
        void* memory = LLAllocator::allocate_node(block_size_, 8);
        memory_block block(memory, block_size_);
        block_size_ = std::size_t(block_size_ * growth_factor());
        return block;
    }

    void
    deallocate_block(memory_block block) noexcept
    {
        LLAllocator::deallocate_node(block.memory, block.size, 8);
    }

    std::size_t
    next_block_size() const noexcept
    {
        return block_size_;
    }

    LLAllocator&
    get_allocator() noexcept
    {
        return *this;
    }

    static float
    growth_factor() noexcept
    {
        static constexpr auto factor = float(Num) / Den;
        return factor;
    }

private:
    std::size_t block_size_ = 0;
};

/**********************************************************************/

/// stores memory block in an intrusive linked list and allows LIFO access
class memory_block_stack
{
public:
    memory_block_stack() noexcept;
    ~memory_block_stack() noexcept;
    memory_block_stack(memory_block_stack&& other) noexcept;
    memory_block_stack& operator=(memory_block_stack&& other) noexcept;
    friend void swap(memory_block_stack& a, memory_block_stack& b) noexcept;

    // the raw allocated block returned from an allocator
    using allocated_mb = memory_block;

    // the inserted block slightly smaller to allow for the fixup value
    using inserted_mb = memory_block;

    // how much an inserted block is smaller
    static const std::size_t implementation_offset;

    void push(allocated_mb block) noexcept;
    allocated_mb pop() noexcept;
    inserted_mb top() const noexcept;
    bool empty() const noexcept;
    bool owns(const void* ptr) const noexcept;

    // O(n) size
    std::size_t size() const noexcept;

private:
    struct node
    {
        node* prev = nullptr;
        std::size_t usable_size = 0;

        node(node* p, std::size_t size) noexcept;

        static const std::size_t div_alignment;
        static const std::size_t mod_offset;
        static const std::size_t offset;
    };

    node* head_ = nullptr;
};

/**********************************************************************/

template <typename BlockAllocator>
class memory_arena : BlockAllocator
{
public:
    using allocator_type = BlockAllocator;

    template <typename... Args>
    explicit memory_arena(std::size_t block_size, Args&&... args)
            : allocator_type(block_size, std::forward<Args>(args)...)
    {
        // empty
    }

    /// Deallocates all memory blocks that where requested back
    /// to the BlockAllocator.
    ~memory_arena() noexcept
    {
        while (!used_.empty())
            allocator_type::deallocate_block(used_.pop());
    }

    memory_arena(memory_arena&& other) noexcept
            : allocator_type(std::move(other))
            , used_(std::move(other.used_))
    {}

    memory_arena&
    operator=(memory_arena&& other) noexcept
    {
        memory_arena tmp(std::move(other));
        swap(*this, tmp);
        return *this;
    }

    friend void
    swap(memory_arena& a, memory_arena& b) noexcept
    {
        std::swap(static_cast<allocator_type&>(a), static_cast<allocator_type&>(b));
        std::swap(a.used_, b.used_);
    }

    memory_block
    allocate_block()
    {
        used_.push(allocator_type::allocate_block());

        auto block = used_.top();
        return block;
    }

    memory_block
    current_block() const noexcept
    {
        return used_.top();
    }

    void
    deallocate_block() noexcept
    {
        get_allocator().deallocate_block(used_.pop());
    }

    bool
    owns(const void* ptr) const noexcept
    {
        return used_.owns(ptr);
    }

    std::size_t
    capacity() const noexcept
    {
        return size();
    }

    std::size_t
    size() const noexcept
    {
        return used_.size();
    }

    std::size_t
    next_block_size() const noexcept
    {
        return allocator_type::next_block_size() - memory_block_stack::implementation_offset;
    }

    allocator_type&
    get_allocator() noexcept
    {
        return *this;
    }

private:
    memory_block_stack used_;
};
