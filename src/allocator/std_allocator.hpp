#pragma once

#include "reference_storage.hpp"
#include <type_traits> // std::true_type
#include <utility> // std::forward


template <typename T, class Allocator>
class std_allocator : reference_storage
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
        using other = std_allocator<U, Allocator>;
    };

    using allocator_type = reference_storage::allocator_type;

    /// ctor used when initializing std containers
    template <typename Alloc>
    std_allocator(Alloc& alloc) noexcept
            : reference_storage(alloc)
    {
        // empty
    }

    std_allocator<value_type, Allocator>
    select_on_container_copy_construction() const
    {
        return *this;
    }

    value_type*
    allocate(size_type n, void* = nullptr)
    {
        if (n == 1)
            return static_cast<value_type*>(get_allocator().allocate_node());
        else
            return static_cast<value_type*>(get_allocator().allocate_array(n));
    }

    void
    deallocate(value_type* ptr, size_type n) noexcept
    {
        if (n == 1)
            get_allocator().deallocate_node(ptr);
        else
            get_allocator().deallocate_array(ptr, n);
    }

    template <typename U, typename... Args>
    void
    construct(U* p, Args&&... args)
    {
        void* mem = p;
        ::new (mem) U(std::forward<Args>(args)...);
    }

    template <typename U>
    void
    destroy(U* p) noexcept
    {
        p->~U();
    }

    size_type
    max_size() const noexcept
    {
        return this->max_array_size() / sizeof(value_type);
    }

    allocator_type&
    get_allocator() noexcept
    {
        return reference_storage::get_allocator();
    }

    allocator_type const&
    get_allocator() const noexcept
    {
        return reference_storage::get_allocator();
    }

private:
    template <typename T1, typename T2, class Impl>
    friend bool operator==(
            std_allocator<T1, Impl> const& lhs, std_allocator<T2, Impl> const& rhs) noexcept;

    template <typename U, class OtherAllocator>
    friend class std_allocator;
};


template <typename T, typename U, class Impl>
bool
operator==(std_allocator<T, Impl> const& lhs, std_allocator<U, Impl> const& rhs) noexcept
{
    return &lhs.get_allocator() == &rhs.get_allocator();
}

template <typename T, typename U, class Impl>
bool
operator!=(std_allocator<T, Impl> const& lhs, std_allocator<U, Impl> const& rhs) noexcept
{
    return !(lhs == rhs);
}
