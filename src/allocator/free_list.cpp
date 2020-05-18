#include "free_list.hpp"
#include "util/assert.hpp"
#include <climits> // CHAR_BIT
#include <cstring> // std::memcpy
#include <utility>


namespace {

    constexpr inline std::size_t
    ilog2(std::uint64_t x)
    {
        return ((sizeof(x) * CHAR_BIT) - __builtin_clzll(x)) - 1;
    }

    // sets stored integer value
    inline void
    set_int(void* address, std::uintptr_t i) noexcept
    {
        DEBUG_ASSERT(address);
        std::memcpy(address, &i, sizeof(std::uintptr_t));
    }

    // reads stored integer value
    inline std::uintptr_t
    get_int(void* address) noexcept
    {
        DEBUG_ASSERT(address != nullptr);
        std::uintptr_t res;
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
        std::uint8_t* prev; // last before
        std::uint8_t* first; // first in
        std::uint8_t* last; // last in
        std::uint8_t* next; // first after

        // number of nodes in the interval
        std::size_t
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
    interval
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
            if (i.last + node_size != i.next) // not continous
            {
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

} // namespace

constexpr std::size_t free_list::min_element_size;
constexpr std::size_t free_list::min_element_alignment;

free_list::free_list(std::size_t node_size) noexcept
        : first_(nullptr)
        , node_size_(node_size > min_element_size ? node_size : min_element_size)
        , capacity_(0u)
{
    // empty
}

free_list::free_list(std::size_t node_size, void* mem, std::size_t size) noexcept
        : free_list(node_size)
{
    insert(mem, size);
}

free_list::free_list(free_list&& other) noexcept
        : first_(other.first_)
        , node_size_(other.node_size_)
        , capacity_(other.capacity_)
{
    other.first_ = nullptr;
    other.capacity_ = 0u;
}

free_list&
free_list::operator=(free_list&& other) noexcept
{
    free_list tmp(std::move(other));
    swap(*this, tmp);
    return *this;
}

void
swap(free_list& a, free_list& b) noexcept
{
    std::swap(a.first_, b.first_);
    std::swap(a.node_size_, b.node_size_);
    std::swap(a.capacity_, b.capacity_);
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
    --capacity_;

    std::uint8_t* mem = first_;
    first_ = list_get_next(first_);
    return mem;
}

void*
free_list::allocate(std::size_t n) noexcept
{
    DEBUG_ASSERT(!empty());
    if (n <= node_size_)
        return allocate();

    interval i = list_search_array(first_, n, node_size_);
    if (i.first == nullptr)
        return nullptr;

    if (i.prev)
        list_set_next(i.prev, i.next); // change next from previous to first after
    else
        first_ = i.next;
    capacity_ -= i.size(node_size_);

    return i.first;
}

void
free_list::deallocate(void* ptr) noexcept
{
    ++capacity_;

    std::uint8_t* node = static_cast<std::uint8_t*>(ptr);
    list_set_next(node, first_);
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

std::size_t
free_list::node_size() const noexcept
{
    return node_size_;
}

std::size_t
free_list::alignment() const noexcept
{
    constexpr static std::size_t max_alignment = alignof(std::max_align_t);
    return node_size_ >= max_alignment ? max_alignment : (1u << ilog2(node_size_));
}

std::size_t
free_list::capacity() const noexcept
{
    return capacity_;
}

bool
free_list::empty() const noexcept
{
    return first_ == nullptr;
}

void
free_list::insert_impl(void* mem, std::size_t size) noexcept
{
    auto no_nodes = size / node_size_;
    DEBUG_ASSERT(no_nodes > 0);

    std::uint8_t* cur = static_cast<std::uint8_t*>(mem);
    for (std::size_t i = 0u; i != no_nodes - 1; ++i) {
        list_set_next(cur, cur + node_size_);
        cur += node_size_;
    }
    list_set_next(cur, first_);
    first_ = static_cast<std::uint8_t*>(mem);

    capacity_ += no_nodes;
}
