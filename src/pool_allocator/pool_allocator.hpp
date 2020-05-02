#pragma once

#include <cstddef>
#include <cstdlib>


template <typename T>
class pool_allocator
{
public:
    using value_type = T;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

public:
    constexpr pool_allocator() noexcept;
    constexpr pool_allocator(pool_allocator const&) noexcept;
    template <typename U>
    constexpr pool_allocator(pool_allocator<U> const&) noexcept;
    ~pool_allocator() noexcept;
    [[nodiscard]] constexpr value_type* allocate(size_type) noexcept;
    constexpr void deallocate(value_type*, size_type);
};


template <typename T, typename U>
constexpr bool operator==(pool_allocator<T> const&, pool_allocator<U> const&) noexcept;

/**********************************************************************/

template <typename T>
constexpr pool_allocator<T>::pool_allocator() noexcept
{}

template <typename T>
constexpr pool_allocator<T>::pool_allocator(pool_allocator const&) noexcept
{}

template <typename T>
template <typename U>
constexpr pool_allocator<T>::pool_allocator(pool_allocator<U> const&) noexcept
{}

template <typename T>
pool_allocator<T>::~pool_allocator() noexcept
{}

template <typename T>
constexpr typename pool_allocator<T>::value_type*
pool_allocator<T>::allocate(pool_allocator<T>::size_type n) noexcept
{
    return static_cast<value_type*>(::malloc(n));
}

template <typename T>
constexpr void
pool_allocator<T>::deallocate(pool_allocator<T>::value_type* ptr, pool_allocator<T>::size_type)
{
    ::free(ptr);
}

template <typename T, typename U>
constexpr bool
operator==(pool_allocator<T> const& lhs, pool_allocator<U> const& rhs) noexcept
{
    return false;
}
