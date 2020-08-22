#pragma once

#include "detail.hpp"
#include "free_list.hpp"
#include "growing_block_allocator.hpp"
#include "lowlevel_allocator.hpp"
#include "memory_arena.hpp"
#include "util/assert.hpp"
#include <cstdlib>
#include <exception> // std::terminate
#include <new>
#include <type_traits>
#include <utility>


class memory_pool
{
public:
    using allocator_type = growing_block_allocator<lowlevel_allocator<malloc_allocator>>;

private:
    memory_arena<allocator_type> arena_;
    free_list free_list_;

public:
    inline memory_pool(std::size_t node_size, std::size_t count) noexcept;
    ~memory_pool() noexcept = default;
    inline memory_pool(memory_pool&&) noexcept;
    memory_pool(memory_pool const&) noexcept = delete;
    inline memory_pool& operator=(memory_pool&&) noexcept;
    memory_pool& operator=(memory_pool const&) noexcept = delete;
    inline void* allocate_node() noexcept;
    inline void* allocate_array(std::size_t n) noexcept;
    inline void deallocate_node(void* ptr) noexcept;
    inline void deallocate_array(void* ptr, std::size_t n) noexcept;
    inline std::size_t node_size() const noexcept;
    inline std::size_t max_used() const noexcept;
    inline std::size_t capacity_left() const noexcept;
    inline std::size_t next_capacity() const noexcept;
    inline allocator_type& get_allocator() noexcept;

private:
    inline void allocate_block() noexcept;
};

/**********************************************************************/

memory_pool::memory_pool(std::size_t node_size, std::size_t count) noexcept
        : arena_(detail::round_up_to_align(node_size) * count)
        , free_list_(detail::round_up_to_align(node_size))
{
    allocate_block();
}

memory_pool::memory_pool(memory_pool&& other) noexcept
        : arena_(std::move(other.arena_))
        , free_list_(std::move(other.free_list_))
{
    // empty
}

memory_pool&
memory_pool::operator=(memory_pool&& other) noexcept
{
    arena_ = std::move(other.arena_);
    free_list_ = std::move(other.free_list_);
    return *this;
}

void*
memory_pool::allocate_node() noexcept
{
    if (free_list_.empty())
        allocate_block();
    DEBUG_ASSERT(!free_list_.empty());
    return free_list_.allocate();
}

void*
memory_pool::allocate_array(std::size_t n) noexcept
{
    void* mem = free_list_.empty() ? nullptr : free_list_.allocate(n * node_size());
    if (mem == nullptr) {
        allocate_block();
        mem = free_list_.allocate(n * node_size());
        if (mem == nullptr)
            std::terminate();
    }
    return mem;
}

void
memory_pool::deallocate_node(void* ptr) noexcept
{
    free_list_.deallocate(ptr);
}

void
memory_pool::deallocate_array(void* ptr, std::size_t n) noexcept
{
    free_list_.deallocate(ptr, n * node_size());
}

std::size_t
memory_pool::node_size() const noexcept
{
    return free_list_.node_size();
}

std::size_t
memory_pool::max_used() const noexcept
{
    return free_list_.max_used();
}

std::size_t
memory_pool::capacity_left() const noexcept
{
    return free_list_.capacity_left() * node_size();
}

std::size_t
memory_pool::next_capacity() const noexcept
{
    return arena_.next_block_size();
}

memory_pool::allocator_type&
memory_pool::get_allocator() noexcept
{
    return arena_.get_allocator();
}

// private

void
memory_pool::allocate_block() noexcept
{
    memory_block mb = arena_.allocate_block();
    free_list_.insert(mb.memory, mb.size);
}
