#pragma once

#include "detail.hpp"
#include "util/assert.hpp"
#include <algorithm> // std::max
#include <climits>   // CHAR_BIT
#include <cstddef>   // std::max_align_t, std::size_t
#include <cstdint>
#include <cstring> // std::memcpy
#include <utility>


namespace detail {

    // sets stored integer value
    inline void
    set_int(void* address, std::uintptr_t i) noexcept
    {
        DEBUG_ASSERT(address != nullptr);
        std::memcpy(address, &i, sizeof(std::uintptr_t));
    }

    // reads stored integer value
    inline std::uintptr_t
    get_int(void* address) noexcept
    {
        DEBUG_ASSERT(address != nullptr);
        std::uintptr_t res = 0;
        std::memcpy(&res, address, sizeof(std::uintptr_t));
        return res;
    }

    // pointer to integer
    inline std::uintptr_t
    to_int(std::uint8_t* ptr) noexcept
    {
        return reinterpret_cast<std::uintptr_t>(ptr);
    }

    // integer to pointer
    inline std::uint8_t*
    from_int(std::uintptr_t i) noexcept
    {
        return reinterpret_cast<std::uint8_t*>(i);
    }

    inline std::uint8_t*
    list_get_next(void* address) noexcept
    {
        DEBUG_ASSERT(address != nullptr);
        return from_int(get_int(address));
    }

    // stores a pointer value
    inline void
    list_set_next(void* address, std::uint8_t* ptr) noexcept
    {
        set_int(address, to_int(ptr));
    }

    // i.e. array
    struct interval
    {
        std::uint8_t* prev = nullptr;  // last before
        std::uint8_t* first = nullptr; // first in
        std::uint8_t* last = nullptr;  // last in
        std::uint8_t* next = nullptr;  // first after

        // number of nodes in the interval
        constexpr std::size_t
        size(std::size_t node_size) const noexcept
        {
            // last is inclusive, so add actual_size to it
            // note: cannot use next, might not be directly after
            auto end = last + node_size;
            DEBUG_ASSERT((end - first) % node_size == 0u);
            return (end - first) / node_size;
        }
    };

    // searches for n consecutive bytes
    // begin and end are the proxy nodes
    // assumes list is not empty
    // similar to list_search_array()
    inline interval
    list_search_array(std::uint8_t* first, std::size_t bytes_needed, std::size_t node_size) noexcept
    {
        interval i;
        i.prev = nullptr;
        i.first = first;
        // i.last/next are used as iterator for the end of the interval
        i.last = first;
        i.next = list_get_next(first);

        auto bytes_so_far = node_size;
        while (i.next) {
            if (i.last + node_size != i.next) { // not continous
                // restart at next
                i.prev = i.last;
                i.first = i.next;
                i.last = i.next;
                i.next = list_get_next(i.last);

                bytes_so_far = node_size;
            } else {
                // extend interval
                auto new_next = list_get_next(i.next);
                i.last = i.next;
                i.next = new_next;

                bytes_so_far += node_size;
                if (bytes_so_far >= bytes_needed)
                    return i;
            }
        }
        // not enough continuous space
        return {nullptr, nullptr, nullptr, nullptr};
    }

} // namespace detail


class free_list
{
public:
    constexpr free_list(std::size_t node_size) noexcept;
    /// calls constructor plus insert
    constexpr free_list(std::size_t node_size, void* mem, std::size_t size) noexcept;
    constexpr free_list(free_list const&) noexcept = delete;
    constexpr free_list& operator=(free_list const&) noexcept = delete;
    constexpr free_list(free_list&&) noexcept;
    constexpr ~free_list() noexcept = default;
    inline free_list& operator=(free_list&&) noexcept;

    inline void insert(void* mem, std::size_t size) noexcept;
    inline void* allocate() noexcept;
    inline void* allocate(std::size_t n) noexcept;
    inline void deallocate(void* ptr) noexcept;
    inline void deallocate(void* ptr, std::size_t n) noexcept;

    constexpr std::size_t node_size() const noexcept;
    constexpr std::size_t capacity_left() const noexcept;
    constexpr std::size_t used() const noexcept;
    constexpr std::size_t max_used() const noexcept;
    constexpr bool empty() const noexcept;

    friend inline void swap(free_list&, free_list&) noexcept;

private:
    inline void insert_impl(void* mem, std::size_t size) noexcept;

private:
    std::uint8_t* first_ = nullptr;
    std::size_t node_size_ = 0;
    std::size_t capacity_left_ = 0;
    std::size_t used_ = 0;
    std::size_t max_used_ = 0; ///< only for stats tracking
};

/**********************************************************************/

constexpr free_list::free_list(std::size_t node_size) noexcept
        : node_size_(node_size > detail::MinElementSize
                        ? detail::round_up_to_align(node_size, detail::DefaultAlignment)
                        : detail::MinElementSize)
{
    // empty
}

constexpr free_list::free_list(std::size_t node_size, void* mem, std::size_t size) noexcept
        : free_list(node_size)
{
    DEBUG_ASSERT(detail::is_aligned(mem, 8));
    insert(mem, size);
}

constexpr free_list::free_list(free_list&& other) noexcept
        : first_(other.first_)
        , node_size_(other.node_size_)
        , capacity_left_(other.capacity_left_)
        , used_(other.used_)
        , max_used_(other.max_used_)
{
    other.first_ = nullptr;
    other.capacity_left_ = 0;
    other.used_ = 0;
    other.max_used_ = 0;
}

free_list&
free_list::operator=(free_list&& other) noexcept
{
    free_list tmp(std::move(other));
    swap(*this, tmp);
    return *this;
}

void
free_list::insert(void* mem, std::size_t size) noexcept
{
    DEBUG_ASSERT(mem != nullptr);
    DEBUG_ASSERT(size != 0);
    insert_impl(mem, size);
}

void*
free_list::allocate() noexcept
{
    DEBUG_ASSERT(!empty());
    --capacity_left_;
    ++used_;
    max_used_ = std::max(max_used_, used_);

    std::uint8_t* mem = first_;
    first_ = detail::list_get_next(first_);

    DEBUG_ASSERT(detail::is_aligned(mem, 8));
    return mem;
}

void*
free_list::allocate(std::size_t n) noexcept
{
    DEBUG_ASSERT(!empty());
    if (n <= node_size_)
        return allocate();

    detail::interval i = detail::list_search_array(first_, n, node_size_);
    if (i.first == nullptr)
        return nullptr;

    if (i.prev)
        detail::list_set_next(i.prev, i.next); // change next from previous to first after
    else
        first_ = i.next;
    capacity_left_ -= i.size(node_size_);
    used_ += i.size(node_size_);
    max_used_ = std::max(max_used_, used_);

    return i.first;
}

void
free_list::deallocate(void* ptr) noexcept
{
    ++capacity_left_;
    --used_;

    auto* node = static_cast<std::uint8_t*>(ptr);
    detail::list_set_next(node, first_);
    first_ = node;
}

void
free_list::deallocate(void* ptr, std::size_t n) noexcept
{
    if (n <= node_size_)
        deallocate(ptr);
    else
        insert_impl(ptr, n);
}

constexpr std::size_t
free_list::node_size() const noexcept
{
    return node_size_;
}

constexpr std::size_t
free_list::capacity_left() const noexcept
{
    return capacity_left_;
}

constexpr std::size_t
free_list::used() const noexcept
{
    return used_;
}

constexpr std::size_t
free_list::max_used() const noexcept
{
    return max_used_;
}

constexpr bool
free_list::empty() const noexcept
{
    return first_ == nullptr;
}

// friend
void
swap(free_list& a, free_list& b) noexcept
{
    std::swap(a.first_, b.first_);
    std::swap(a.node_size_, b.node_size_);
    std::swap(a.capacity_left_, b.capacity_left_);
    std::swap(a.used_, b.used_);
    std::swap(a.max_used_, b.max_used_);
}

void
free_list::insert_impl(void* mem, std::size_t size) noexcept
{
    DEBUG_ASSERT(detail::is_aligned(mem, 8));
    std::size_t const no_nodes = size / node_size_;
    DEBUG_ASSERT(no_nodes > 0);

    auto* cur = static_cast<std::uint8_t*>(mem);
    for (std::size_t i = 0u; i != no_nodes - 1; ++i) {
        DEBUG_ASSERT(detail::is_aligned(cur, 8));
        detail::list_set_next(cur, cur + node_size_);
        cur += node_size_;
    }
    detail::list_set_next(cur, first_);
    first_ = static_cast<std::uint8_t*>(mem);

    capacity_left_ += no_nodes;
}
