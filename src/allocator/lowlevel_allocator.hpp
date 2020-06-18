#pragma once

#include <sys/mman.h>
#include <cstdlib> // ::posix_memalign
#include <memory>
#include <new>


class bad_allocation : public std::bad_alloc
{
public:
    bad_allocation() noexcept = default;

    constexpr char const*
    what() const noexcept override
    {
        return "lowlevel_allocator bad allocation";
    }
};


template <typename Functor>
class lowlevel_allocator
{
public:
    constexpr lowlevel_allocator() noexcept = default;
    constexpr lowlevel_allocator(lowlevel_allocator&&) noexcept = default;
    constexpr ~lowlevel_allocator() noexcept = default;
    lowlevel_allocator(lowlevel_allocator const&) noexcept = delete;

    constexpr lowlevel_allocator&
    operator=(lowlevel_allocator&&) noexcept
    {
        return *this;
    }

    lowlevel_allocator& operator=(lowlevel_allocator const&) = delete;

    [[nodiscard]] constexpr void*
    allocate_node(std::size_t size, std::size_t alignment) const
    {
        void* memory = Functor::allocate(size, alignment);
        if (memory == nullptr)
            throw bad_allocation();

        return memory;
    }

    constexpr void
    deallocate_node(void* node, std::size_t size, std::size_t alignment) const
            noexcept(noexcept(Functor::deallocate(node, size, alignment)))
    {
        Functor::deallocate(node, size, alignment);
    }

    constexpr std::size_t
    max_node_size() const noexcept
    {
        return Functor::max_node_size();
    }
};

struct malloc_allocator
{
    [[nodiscard]] static void*
    allocate(std::size_t size, std::size_t /*alignment*/) noexcept
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
    static constexpr std::size_t DefaultAlignment = 8;

    [[nodiscard]] static void*
    allocate(std::size_t size, std::size_t alignment = DefaultAlignment) noexcept
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
    static constexpr std::size_t DefaultAlignment = 8;

    [[nodiscard]] static void*
    allocate(std::size_t size, std::size_t alignment = DefaultAlignment) noexcept
    {
        void* ptr = nullptr;
        ::posix_memalign(&ptr, alignment, size);
        ::madvise(ptr, size, MADV_HUGEPAGE);
        return ptr;
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

struct mmap_allocator
{
    [[nodiscard]] static void*
    allocate(std::size_t size, std::size_t /*alignment*/) noexcept
    {
        return ::mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    }

    static void
    deallocate(void* ptr, std::size_t size, std::size_t /*alignment*/) noexcept
    {
        ::munmap(ptr, size);
    }

    static std::size_t
    max_node_size() noexcept
    {
        return std::allocator_traits<std::allocator<char>>::max_size({});
    }
};
