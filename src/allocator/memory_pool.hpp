#pragma once

#include "free_list.hpp"
#include "memory_arena.hpp"
#include "common/assert.hpp"
#include <cstdlib>
#include <new>
#include <type_traits>
#include <utility>


class memory_pool
{
public:
    using allocator_type = growing_block_allocator<lowlevel_allocator<malloc_allocator>>;

    static constexpr std::size_t min_node_size = free_list::min_element_size;

    template <typename... Args>
    memory_pool(std::size_t node_size, std::size_t block_size, Args&&... args)
            : arena_(block_size, std::forward<Args>(args)...)
            , free_list_(node_size)
    {
        allocate_block();
    }

    ~memory_pool() noexcept
    {
        // empty
    }

    memory_pool(memory_pool&& other) noexcept
            : arena_(std::move(other.arena_))
            , free_list_(std::move(other.free_list_))
    {
        // empty
    }

    memory_pool&
    operator=(memory_pool&& other) noexcept
    {
        arena_ = std::move(other.arena_);
        free_list_ = std::move(other.free_list_);
        return *this;
    }

    void*
    allocate_node()
    {
        if (free_list_.empty())
            allocate_block();
        DEBUG_ASSERT(!free_list_.empty());
        return free_list_.allocate();
    }

    void*
    allocate_array(std::size_t n)
    {
        void* mem = free_list_.empty() ? nullptr : free_list_.allocate(n * node_size());
        if (mem == nullptr) {
            allocate_block();
            mem = free_list_.allocate(n * node_size());
            if (mem == nullptr)
                throw std::bad_alloc();
        }
        return mem;
    }

    void
    deallocate_node(void* ptr) noexcept
    {
        free_list_.deallocate(ptr);
    }

    void
    deallocate_array(void* ptr, std::size_t n) noexcept
    {
        free_list_.deallocate(ptr, n * node_size());
    }

    std::size_t
    node_size() const noexcept
    {
        return free_list_.node_size();
    }

    std::size_t
    capacity_left() const noexcept
    {
        return free_list_.capacity() * node_size();
    }

    std::size_t
    next_capacity() const noexcept
    {
        return arena_.next_block_size();
    }

    allocator_type&
    get_allocator() noexcept
    {
        return arena_.get_allocator();
    }

private:
    void
    allocate_block()
    {
        memory_block mb = arena_.allocate_block();
        free_list_.insert(static_cast<char*>(mb.memory), mb.size);
    }

private:
    memory_arena<growing_block_allocator<lowlevel_allocator<malloc_allocator>>> arena_;
    free_list free_list_;
};

constexpr std::size_t memory_pool::min_node_size;
