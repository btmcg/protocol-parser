#pragma once

#include "allocator_storage.hpp"
#include "memory_pool.hpp"
#include <type_traits> // std::true_type
#include <utility> // std::forward


/// Wraps a \concept{concept_rawallocator,RawAllocator} and makes it a "normal" \c Allocator.
/// It allows using a \c RawAllocator anywhere a \c Allocator is required.
/// \ingroup memory adapter
template <typename T, class RawAllocator>
class std_allocator : reference_storage
{
    using alloc_reference = reference_storage;

public:
    //=== typedefs ===//
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    using propagate_on_container_swap = std::true_type;
    using propagate_on_container_move_assignment = std::true_type;
    using propagate_on_container_copy_assignment = std::true_type;

    template <typename U>
    struct rebind
    {
        using other = std_allocator<U, RawAllocator>;
    };

    using allocator_type = typename alloc_reference::allocator_type;

    //=== constructor ===//
    /// \effects Default constructs it by storing a default constructed, stateless \c RawAllocator
    /// inside the reference. \requires The \c RawAllocator type is stateless, otherwise the body of
    /// this function will not compile.
    // std_allocator() noexcept
    //         : alloc_reference(allocator_type{})
    // {}

    /// \effects Creates it from a reference to a \c RawAllocator.
    /// It will store an \ref allocator_reference to it.
    /// \requires The expression <tt>allocator_reference<RawAllocator>(alloc)</tt> is well-formed,
    /// that is either \c RawAlloc is the same as \c RawAllocator or \c RawAllocator is the tag type
    /// \ref any_allocator. If the requirement is not fulfilled this function does not participate
    /// in overload resolution. \note The caller has to ensure that the lifetime of the \c
    /// RawAllocator is at least as long as the lifetime of this \ref std_allocator object.
    template <class RawAlloc>
    std_allocator(RawAlloc& alloc, decltype(alloc_reference(alloc), int()) = 0) noexcept
            : alloc_reference(alloc)
    {}

    /// \effects Creates it from a stateless, temporary \c RawAllocator object.
    /// It will not store a reference but create it on the fly.
    /// \requires The \c RawAllocator is stateless
    /// and the expression <tt>allocator_reference<RawAllocator>(alloc)</tt> is well-formed as
    /// above, otherwise this function does not participate in overload resolution.
    template <class RawAlloc>
    std_allocator(const RawAlloc& alloc, decltype(alloc_reference(alloc))) noexcept
            : alloc_reference(alloc)
    {}

    /// \effects Creates it from another \ref allocator_reference using the same allocator type.
    std_allocator(const alloc_reference& alloc) noexcept
            : alloc_reference(alloc)
    {}

    /// \details Implicit conversion from any other \ref allocator_storage is forbidden
    /// to prevent accidentally wrapping another \ref allocator_storage inside a \ref
    /// allocator_reference.
    template <class StoragePolicy>
    std_allocator(const allocator_storage<StoragePolicy>&) = delete;

    /// @{
    /// \effects Creates it from another \ref std_allocator allocating a different type.
    /// This is required by the \c Allcoator concept and simply takes the same \ref
    /// allocator_reference.
    template <typename U>
    std_allocator(const std_allocator<U, RawAllocator>& alloc) noexcept
            : alloc_reference(alloc)
    {}

    template <typename U>
    std_allocator(std_allocator<U, RawAllocator>& alloc) noexcept
            : alloc_reference(alloc)
    {}
    /// @}

    /// \returns A copy of the allocator.
    /// This is required by the \c Allocator concept and forwards to the \ref propagation_traits.
    std_allocator<T, RawAllocator>
    select_on_container_copy_construction() const
    {
        return *this;
    }

    //=== allocation/deallocation ===//
    /// \effects Allocates memory using the underlying \concept{concept_rawallocator,RawAllocator}.
    /// If \c n is \c 1, it will call <tt>allocate_node(sizeof(T), alignof(T))</tt>,
    /// otherwise <tt>allocate_array(n, sizeof(T), alignof(T))</tt>.
    /// \returns A pointer to a memory block suitable for \c n objects of type \c T.
    /// \throws Anything thrown by the \c RawAllocator.
    pointer
    allocate(size_type n, void* = nullptr)
    {
        return static_cast<pointer>(allocate_impl(n));
    }

    /// \effects Deallcoates memory using the underlying
    /// \concept{concept_rawallocator,RawAllocator}. It will forward to the deallocation function in
    /// the same way as in \ref allocate(). \requires The pointer must come from a previous call to
    /// \ref allocate() with the same \c n on this object or any copy of it.
    void
    deallocate(pointer p, size_type n) noexcept
    {
        deallocate_impl(p, n);
    }

    //=== construction/destruction ===//
    /// \effects Creates an object of type \c U at given address using the passed arguments.
    template <typename U, typename... Args>
    void
    construct(U* p, Args&&... args)
    {
        void* mem = p;
        ::new (mem) U(forward<Args>(args)...);
    }

    /// \effects Calls the destructor for an object of type \c U at given address.
    template <typename U>
    void
    destroy(U* p) noexcept
    {
        // This is to avoid a MSVS 2015 'unreferenced formal parameter' warning
        (void)p;
        p->~U();
    }

    //=== getter ===//
    /// \returns The maximum size for an allocation which is <tt>max_array_size() /
    /// sizeof(value_type)</tt>. This is only an upper bound, not the exact maximum.
    size_type
    max_size() const noexcept
    {
        return this->max_array_size() / sizeof(value_type);
    }

    /// @{
    /// \effects Returns a reference to the referenced allocator.
    /// \returns For stateful allocators: A (\c const) reference to the stored allocator.
    /// For stateless allocators: A temporary constructed allocator.
    auto
    get_allocator() noexcept -> decltype(std::declval<alloc_reference>().get_allocator())
    {
        return alloc_reference::get_allocator();
    }

    auto
    get_allocator() const noexcept
            -> decltype(std::declval<const alloc_reference>().get_allocator())
    {
        return alloc_reference::get_allocator();
    }
    /// @}

private:
    // any_allocator_reference: use virtual function which already does a dispatch on node/array
    void* allocate_impl(size_type /*n*/)
    {
        // return get_allocator()->allocate_impl(n, sizeof(T), alignof(T));
        return get_allocator().allocate_node();
    }

    void
    deallocate_impl(void* ptr, size_type /*n*/)
    {
        // get_allocator()->deallocate_impl(ptr, n, sizeof(T), alignof(T));
        get_allocator().deallocate_node(ptr);
    }

    template <typename U> // stateful
    bool
    equal_to_impl(const std_allocator<U, RawAllocator>& other) const noexcept
    {
        return &get_allocator() == &other.get_allocator();
    }

    template <typename U> // shared
    bool
    equal_to(const std_allocator<U, RawAllocator>& other) const noexcept
    {
        return get_allocator() == other.get_allocator();
    }

    template <typename T1, typename T2, class Impl>
    friend bool operator==(
            const std_allocator<T1, Impl>& lhs, const std_allocator<T2, Impl>& rhs) noexcept;

    template <typename U, class OtherRawAllocator>
    friend class std_allocator;
};
