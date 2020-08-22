#pragma once

#include "reference_storage.hpp"
#include <type_traits> // std::true_type
#include <utility>     // std::forward


template <typename T>
class mp_allocator : private reference_storage
{
public:
    using value_type = T;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using propagate_on_container_swap = std::true_type;
    using propagate_on_container_move_assignment = std::true_type;
    using propagate_on_container_copy_assignment = std::true_type;

    template <typename U>
    struct rebind
    {
        using other = mp_allocator<U>;
    };

    using allocator_type = reference_storage::allocator_type;

    /// ctor used when initializing std containers
    template <typename Alloc>
    mp_allocator(Alloc& alloc) noexcept;
    mp_allocator<T> select_on_container_copy_construction() const;
    value_type* allocate(size_type n, void* = nullptr);
    void deallocate(value_type* ptr, size_type n) noexcept;
    template <typename U, typename... Args>
    void construct(U* p, Args&&... args);
    template <typename U>
    void destroy(U* p) noexcept;
    size_type max_size() const noexcept;
    allocator_type& get_allocator() noexcept;
    allocator_type const& get_allocator() const noexcept;

private:
    template <typename T1, typename T2>
    friend bool operator==(mp_allocator<T1> const& lhs, mp_allocator<T2> const& rhs) noexcept;

    template <typename U>
    friend class mp_allocator;
};

template <typename T>
template <typename Alloc>
mp_allocator<T>::mp_allocator(Alloc& alloc) noexcept
        : reference_storage(alloc)
{
    // empty
}

template <typename T>
mp_allocator<T>
mp_allocator<T>::select_on_container_copy_construction() const
{
    return *this;
}

template <typename T>
typename mp_allocator<T>::value_type*
mp_allocator<T>::allocate(size_type n, void*)
{
    if (n == 1)
        return static_cast<value_type*>(get_allocator().allocate_node());
    else
        return static_cast<value_type*>(get_allocator().allocate_array(n));
}

template <typename T>
void
mp_allocator<T>::deallocate(value_type* ptr, size_type n) noexcept
{
    if (n == 1)
        get_allocator().deallocate_node(ptr);
    else
        get_allocator().deallocate_array(ptr, n);
}

template <typename T>
template <typename U, typename... Args>
void
mp_allocator<T>::construct(U* p, Args&&... args)
{
    void* mem = p;
    ::new (mem) U(std::forward<Args>(args)...);
}

template <typename T>
template <typename U>
void
mp_allocator<T>::destroy(U* p) noexcept
{
    p->~U();
}

template <typename T>
typename mp_allocator<T>::size_type
mp_allocator<T>::max_size() const noexcept
{
    return this->max_array_size() / sizeof(value_type);
}

template <typename T>
typename mp_allocator<T>::allocator_type&
mp_allocator<T>::get_allocator() noexcept
{
    return reference_storage::get_allocator();
}

template <typename T>
typename mp_allocator<T>::allocator_type const&
mp_allocator<T>::get_allocator() const noexcept
{
    return reference_storage::get_allocator();
}


template <typename T, typename U>
bool
operator==(mp_allocator<T> const& lhs, mp_allocator<U> const& rhs) noexcept
{
    return &lhs.get_allocator() == &rhs.get_allocator();
}

template <typename T, typename U>
bool
operator!=(mp_allocator<T> const& lhs, mp_allocator<U> const& rhs) noexcept
{
    return !(lhs == rhs);
}
