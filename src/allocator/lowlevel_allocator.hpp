#pragma once

#include "util/assert.hpp"
#include <fmt/format.h>
#include <cerrno>
#include <cstdlib> // std::aligned_alloc
#include <memory>
#include <new>


class bad_allocation : public std::bad_alloc
{
public:
    bad_allocation() noexcept = default;

    char const*
    what() const noexcept override
    {
        return "lowlevel_allocator bad allocation";
    }
};


template <typename Functor>
class lowlevel_allocator
{
public:
    lowlevel_allocator() noexcept
    {
        // empty
    }

    lowlevel_allocator(lowlevel_allocator&&) noexcept
    {
        // empty
    }

    ~lowlevel_allocator() noexcept
    {
        // empty
    }

    lowlevel_allocator&
    operator=(lowlevel_allocator&&) noexcept
    {
        return *this;
    }

    [[nodiscard]] void*
    allocate_node(std::size_t size, std::size_t alignment)
    {
        void* memory = Functor::allocate(size, alignment);
        if (memory == nullptr)
            throw bad_allocation();

        return memory;
    }

    void
    deallocate_node(void* node, std::size_t size, std::size_t alignment) noexcept
    {
        Functor::deallocate(node, size, alignment);
    }

    std::size_t
    max_node_size() const noexcept
    {
        return Functor::max_node_size();
    }
};

struct malloc_allocator
{
    [[nodiscard]] static void*
    allocate(std::size_t size, std::size_t /*alignment = 8*/) noexcept
    {
        return std::malloc(size);
    }

    static void
    deallocate(void* ptr, std::size_t /*size*/, std::size_t /*alignment*/) noexcept
    {
        std::free(ptr);
    }

    static std::size_t
    max_node_size() noexcept
    {
        return std::allocator_traits<std::allocator<char>>::max_size({});
    }
};

struct new_allocator
{
    [[nodiscard]] static void*
    allocate(std::size_t size, std::size_t alignment = 8) noexcept
    {
        return ::operator new(size, static_cast<std::align_val_t>(alignment), std::nothrow);
    }

    static void
    deallocate(void* ptr, std::size_t /*size*/, std::size_t alignment) noexcept
    {
        ::operator delete(ptr, static_cast<std::align_val_t>(alignment), std::nothrow);
    }

    static std::size_t
    max_node_size() noexcept
    {
        return std::allocator_traits<std::allocator<char>>::max_size({});
    }
};

struct posix_allocator
{
    [[nodiscard]] static void*
    allocate(std::size_t size, std::size_t alignment = 8) noexcept
    {
        DEBUG_ASSERT(size % alignment == 0);
        return std::aligned_alloc(alignment, size);
    }

    static void
    deallocate(void* ptr, std::size_t /*size*/, std::size_t /*alignment*/) noexcept
    {
        std::free(ptr);
    }

    static std::size_t
    max_node_size() noexcept
    {
        return std::allocator_traits<std::allocator<char>>::max_size({});
    }
};
