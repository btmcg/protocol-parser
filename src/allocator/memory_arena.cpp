#include "memory_arena.hpp"
#include <cstddef> // std::max_align_t


// whether or not an alignment is valid, i.e. a power of two not zero
constexpr bool
is_valid_alignment(std::size_t alignment) noexcept
{
    return alignment && (alignment & (alignment - 1)) == 0u;
}

// whether or not the pointer is aligned for given alignment
// alignment must be valid
bool
is_aligned(void* ptr, std::size_t alignment) noexcept
{
    DEBUG_ASSERT(is_valid_alignment(alignment));
    auto address = reinterpret_cast<std::uintptr_t>(ptr);
    return address % alignment == 0u;
}

constexpr std::size_t max_alignment = alignof(std::max_align_t);

/**********************************************************************/

memory_block::memory_block() noexcept
        : memory(nullptr)
        , size(0)
{
    // empty
}

memory_block::memory_block(void* mem, std::size_t s) noexcept
        : memory(mem)
        , size(s)
{
    // empty
}

memory_block::memory_block(void* begin, void* end) noexcept
        : memory_block(begin,
                static_cast<std::size_t>(
                        static_cast<std::uint8_t*>(end) - static_cast<std::uint8_t*>(begin)))
{
    // empty
}

bool
memory_block::contains(void const* address) const noexcept
{
    auto mem = static_cast<const char*>(memory);
    auto addr = static_cast<const char*>(address);
    return addr >= mem && addr < mem + size;
}

/**********************************************************************/

constexpr std::size_t memory_block_stack::node::div_alignment
        = sizeof(memory_block_stack::node) / max_alignment;
constexpr std::size_t memory_block_stack::node::mod_offset
        = sizeof(memory_block_stack::node) % max_alignment != 0u;
constexpr std::size_t memory_block_stack::node::offset
        = (div_alignment + mod_offset) * max_alignment;

const std::size_t memory_block_stack::implementation_offset = memory_block_stack::node::offset;

memory_block_stack::memory_block_stack() noexcept
        : head_(nullptr)
{}

memory_block_stack::~memory_block_stack() noexcept
{}

memory_block_stack::memory_block_stack(memory_block_stack&& other) noexcept
        : head_(other.head_)
{
    other.head_ = nullptr;
}

memory_block_stack&
memory_block_stack::operator=(memory_block_stack&& other) noexcept
{
    memory_block_stack tmp(std::move(other));
    swap(*this, tmp);
    return *this;
}

bool
memory_block_stack::empty() const noexcept
{
    return head_ == nullptr;
}

void
swap(memory_block_stack& a, memory_block_stack& b) noexcept
{
    std::swap(a.head_, b.head_);
}

void
memory_block_stack::push(allocated_mb block) noexcept
{
    DEBUG_ASSERT(is_aligned(block.memory, max_alignment));
    auto next = ::new (block.memory) node(head_, block.size - node::offset);
    head_ = next;
}

memory_block_stack::allocated_mb
memory_block_stack::pop() noexcept
{
    DEBUG_ASSERT(head_);
    auto to_pop = head_;
    head_ = head_->prev;
    return {to_pop, to_pop->usable_size + node::offset};
}

bool
memory_block_stack::owns(const void* ptr) const noexcept
{
    auto address = static_cast<const char*>(ptr);
    for (auto cur = head_; cur; cur = cur->prev) {
        auto mem = static_cast<char*>(static_cast<void*>(cur));
        if (address >= mem && address < mem + cur->usable_size)
            return true;
    }
    return false;
}

std::size_t
memory_block_stack::size() const noexcept
{
    std::size_t res = 0u;
    for (auto cur = head_; cur; cur = cur->prev)
        ++res;
    return res;
}

memory_block_stack::inserted_mb
memory_block_stack::top() const noexcept
{
    DEBUG_ASSERT(head_);
    auto mem = static_cast<void*>(head_);
    return {static_cast<char*>(mem) + node::offset, head_->usable_size};
}

memory_block_stack::node::node(node* prv, std::size_t sz) noexcept
        : prev(prv)
        , usable_size(sz)
{
    // empty
}
