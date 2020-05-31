#pragma once

#include "memory_block.hpp"
#include <cstddef> // std::size_t
#include <cstdint>


/// stores memory block in an intrusive linked list and allows LIFO
/// access
class memory_block_stack
{
private:
    struct node
    {
        node* prev = nullptr;
        std::size_t usable_size = 0;

        constexpr node(node* p, std::size_t s) noexcept
                : prev(p)
                , usable_size(s)
        {
            // empty
        }

        static const std::size_t div_alignment;
        static const std::size_t mod_offset;
        static const std::size_t offset;
    };

private:
    node* head_ = nullptr;

public:
    constexpr memory_block_stack() noexcept = default;
    constexpr ~memory_block_stack() noexcept = default;

    memory_block_stack(memory_block_stack&& other) noexcept;
    memory_block_stack& operator=(memory_block_stack&& other) noexcept;

    /// the raw allocated block returned from an allocator
    using allocated_mb = memory_block;

    /// the inserted block slightly smaller to allow for the fixup value
    using inserted_mb = memory_block;

    /// how much an inserted block is smaller
    static const std::size_t implementation_offset;

    void push(allocated_mb block) noexcept;
    allocated_mb pop() noexcept;

    constexpr inserted_mb top() const noexcept;
    constexpr bool owns(void const* ptr) const noexcept;

    /// O(n) size
    constexpr std::size_t size() const noexcept;
    constexpr bool empty() const noexcept;

    friend void swap(memory_block_stack& a, memory_block_stack& b) noexcept;
};

/**********************************************************************/

constexpr memory_block_stack::inserted_mb
memory_block_stack::top() const noexcept
{
    if (empty())
        return memory_block(nullptr, 0);

    void* mem = static_cast<void*>(head_);
    return memory_block(
            static_cast<std::uint8_t*>(mem) + implementation_offset, head_->usable_size);
}

constexpr bool
memory_block_stack::empty() const noexcept
{
    return head_ == nullptr;
}

constexpr std::size_t
memory_block_stack::size() const noexcept
{
    std::size_t res = 0u;
    for (node* cur = head_; cur != nullptr; cur = cur->prev)
        ++res;
    return res;
}

constexpr bool
memory_block_stack::owns(void const* ptr) const noexcept
{
    auto address = static_cast<std::uint8_t const*>(ptr);
    for (node const* cur = head_; cur != nullptr; cur = cur->prev) {
        std::uint8_t const* mem = static_cast<decltype(mem)>(static_cast<void const*>(cur));
        if (address >= mem && address < mem + cur->usable_size)
            return true;
    }
    return false;
}
