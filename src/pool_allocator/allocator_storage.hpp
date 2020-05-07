#pragma once

#include "allocator_traits.hpp"
#include "memory_pool.hpp"

// move - taken from http://stackoverflow.com/a/7518365
template <typename T>
typename std::remove_reference<T>::type&&
move(T&& arg) noexcept
{
    return static_cast<typename std::remove_reference<T>::type&&>(arg);
}

// forward - taken from http://stackoverflow.com/a/27501759
template <class T>
T&&
forward(typename std::remove_reference<T>::type& t) noexcept
{
    return static_cast<T&&>(t);
}
template <class T>
T&&
forward(typename std::remove_reference<T>::type&& t) noexcept
{
    static_assert(!std::is_lvalue_reference<T>::value, "Can not forward an rvalue as an lvalue.");
    return static_cast<T&&>(t);
}


template <class StoragePolicy>
class allocator_storage : StoragePolicy
{
    using traits = allocator_traits<typename StoragePolicy::allocator_type>;

public:
    using allocator_type = typename StoragePolicy::allocator_type;
    using storage_policy = StoragePolicy;
    using is_stateful = typename traits::is_stateful;

    /// \effects Creates it by default-constructing the \c StoragePolicy.
    /// \requires The \c StoragePolicy must be default-constructible.
    /// \notes The default constructor may create an invalid allocator storage not associated with
    /// any allocator. If that is the case, it must not be used.
    allocator_storage() = default;

    /// \effects Creates it by passing it an allocator.
    /// The allocator will be forwarded to the \c StoragePolicy, it decides whether it will be
    /// moved, its address stored or something else. \requires The expression <tt>new
    /// storage_policy(std::forward<Alloc>(alloc))</tt> must be well-formed, otherwise this
    /// constructor does not participate in overload resolution.
    template <class Alloc,
            typename std::enable_if<
                    (!std::is_base_of<allocator_storage, typename std::decay<Alloc>::type>::value),
                    int>::type
            = 0>
    allocator_storage(
            Alloc&& alloc, decltype(new storage_policy(forward<Alloc>(alloc))), int() = nullptr)
            : storage_policy(forward<Alloc>(alloc))
    {}

    /// \effects Creates it by passing it another \c allocator_storage with a different \c
    /// StoragePolicy but the same \c Mutex type. Initializes it with the result of \c
    /// other.get_allocator(). \requires The expression <tt>new
    /// storage_policy(other.get_allocator())</tt> must be well-formed, otherwise this constructor
    /// does not participate in overload resolution.
    template <class OtherPolicy>
    allocator_storage(const allocator_storage<OtherPolicy>& other,
            decltype(new storage_policy(other.get_allocator())), int() = nullptr)
            : storage_policy(other.get_allocator())
    {}

    /// @{
    /// \effects Moves the \c allocator_storage object.
    /// A moved-out \c allocator_storage object must still store a valid allocator object.
    allocator_storage(allocator_storage&& other) noexcept
            : storage_policy(move(other))
    {}

    allocator_storage&
    operator=(allocator_storage&& other) noexcept
    {
        storage_policy::operator=(move(other));
        return *this;
    }
    /// @}

    /// @{
    /// \effects Copies the \c allocator_storage object.
    /// \requires The \c StoragePolicy must be copyable.
    allocator_storage(const allocator_storage&) = default;
    allocator_storage& operator=(const allocator_storage&) = default;
    /// @}

    void*
    allocate_node(std::size_t size, std::size_t alignment)
    {
        auto&& alloc = get_allocator();
        return traits::allocate_node(alloc, size, alignment);
    }

    void*
    allocate_array(std::size_t count, std::size_t size, std::size_t alignment)
    {
        auto&& alloc = get_allocator();
        return traits::allocate_array(alloc, count, size, alignment);
    }

    void
    deallocate_node(void* ptr, std::size_t size, std::size_t alignment) noexcept
    {
        auto&& alloc = get_allocator();
        traits::deallocate_node(alloc, ptr, size, alignment);
    }

    void
    deallocate_array(void* ptr, std::size_t count, std::size_t size, std::size_t alignment) noexcept
    {
        auto&& alloc = get_allocator();
        traits::deallocate_array(alloc, ptr, count, size, alignment);
    }

    std::size_t
    max_node_size() const
    {
        auto&& alloc = get_allocator();
        return traits::max_node_size(alloc);
    }

    std::size_t
    max_array_size() const
    {
        auto&& alloc = get_allocator();
        return traits::max_array_size(alloc);
    }

    std::size_t
    max_alignment() const
    {
        auto&& alloc = get_allocator();
        return traits::max_alignment(alloc);
    }

    auto
    get_allocator() noexcept -> decltype(std::declval<storage_policy>().get_allocator())
    {
        return storage_policy::get_allocator();
    }

    auto
    get_allocator() const noexcept -> decltype(std::declval<const storage_policy>().get_allocator())
    {
        return storage_policy::get_allocator();
    }


    bool
    is_composable() const noexcept
    {
        return StoragePolicy::is_composable();
    }
};
