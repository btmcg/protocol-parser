#include "free_list.hpp"
#include "common/assert.hpp"
#include <climits>
#include <cstdint>
#include <cstring>
#include <utility>


namespace {

    constexpr std::size_t max_alignment = alignof(std::max_align_t);

    inline std::size_t
    ilog2_base(std::uint64_t x)
    {
        unsigned long long value = x;
        return sizeof(value) * CHAR_BIT - __builtin_clzll(value);
    }

    inline std::size_t
    ilog2(std::uint64_t x)
    {
        return ilog2_base(x) - 1;
    }

    std::size_t
    alignment_for(std::size_t size) noexcept
    {
        return size >= max_alignment ? max_alignment : (std::size_t(1) << ilog2(size));
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
        DEBUG_ASSERT(address);
        std::uintptr_t res;
        std::memcpy(&res, address, sizeof(std::uintptr_t));
        return res;
    }

    // pointer to integer
    inline std::uintptr_t
    to_int(char* ptr) noexcept
    {
        return reinterpret_cast<std::uintptr_t>(ptr);
    }


    // integer to pointer
    inline char*
    from_int(std::uintptr_t i) noexcept
    {
        return reinterpret_cast<char*>(i);
    }


    inline char*
    list_get_next(void* address) noexcept
    {
        return from_int(get_int(address));
    }

    // stores a pointer value
    inline void
    list_set_next(void* address, char* ptr) noexcept
    {
        set_int(address, to_int(ptr));
    }


    // i.e. array
    struct interval
    {
        char* prev; // last before
        char* first; // first in
        char* last; // last in
        char* next; // first after

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
    list_search_array(char* first, std::size_t bytes_needed, std::size_t node_size) noexcept
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
{}

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
    DEBUG_ASSERT(mem);

    insert_impl(mem, size);
}

void*
free_list::allocate() noexcept
{
    DEBUG_ASSERT(!empty());
    --capacity_;

    auto mem = first_;
    first_ = list_get_next(first_);
    return mem;
}

void*
free_list::allocate(std::size_t n) noexcept
{
    DEBUG_ASSERT(!empty());
    if (n <= node_size_)
        return allocate();

    auto actual_size = node_size_ + 2 * fence_size();

    auto i = list_search_array(first_, n + 2 * fence_size(), actual_size);
    if (i.first == nullptr)
        return nullptr;

    if (i.prev)
        list_set_next(i.prev, i.next); // change next from previous to first after
    else
        first_ = i.next;
    capacity_ -= i.size(actual_size);

    return i.first;
}

void
free_list::deallocate(void* ptr) noexcept
{
    ++capacity_;

    auto node = static_cast<char*>(ptr);
    list_set_next(node, first_);
    first_ = node;
}

void
free_list::deallocate(void* ptr, std::size_t n) noexcept
{
    if (n <= node_size_)
        deallocate(ptr);
    else {
        auto mem = ptr;
        insert_impl(mem, n + 2 * fence_size());
    }
}

std::size_t
free_list::alignment() const noexcept
{
    return alignment_for(node_size_);
}

std::size_t
free_list::fence_size() const noexcept
{
    return 0u;
}

void
free_list::insert_impl(void* mem, std::size_t size) noexcept
{
    auto actual_size = node_size_ + 2 * fence_size();
    auto no_nodes = size / actual_size;
    DEBUG_ASSERT(no_nodes > 0);

    auto cur = static_cast<char*>(mem);
    for (std::size_t i = 0u; i != no_nodes - 1; ++i) {
        list_set_next(cur, cur + actual_size);
        cur += actual_size;
    }
    list_set_next(cur, first_);
    first_ = static_cast<char*>(mem);

    capacity_ += no_nodes;
}


// #include "free_list.hpp"
// #include "common/assert.hpp"
// #include <cstring> // std::memcpy


// free_list::free_list(std::size_t node_size) noexcept
//         : first_(nullptr)
//         , node_size_(node_size > min_element_size ? node_size : min_element_size)
//         , capacity_(0)
// {
//     // empty
// }

// free_list::free_list(std::size_t node_size, void* addr, std::size_t size) noexcept
//         : first_(nullptr)
//         , node_size_(node_size > min_element_size ? node_size : min_element_size)
//         , capacity_(0)
// {
//     insert(addr, size);
// }

// free_list::free_list(free_list&& rhs) noexcept
//         : first_(rhs.first_)
//         , node_size_(rhs.node_size_)
//         , capacity_(rhs.capacity_)
// {
//     rhs.first_ = nullptr;
//     rhs.capacity_ = 0;
// }

// bool
// free_list::empty() const noexcept
// {
//     return first_ == nullptr;
// }

// std::size_t
// free_list::capacity() const noexcept
// {
//     return capacity_;
// }

// std::size_t
// free_list::node_size() const noexcept
// {
//     return node_size_;
// }

// void
// free_list::insert(void* addr, std::size_t size) noexcept
// {
//     std::size_t const actual_size = node_size_;
//     std::size_t const num_nodes = size / actual_size;
//     DEBUG_ASSERT(num_nodes > 0);

//     std::byte* cur = static_cast<decltype(cur)>(addr);
//     for (std::size_t i = 0; i < num_nodes; ++i) {
//         set_next(cur, cur + actual_size);
//         cur += actual_size;
//     }
//     set_next(cur, first_);
//     first_ = static_cast<decltype(first_)>(addr);
//     capacity_ = num_nodes;
// }

// void*
// free_list::allocate() noexcept
// {
//     if (capacity() == 0)
//         return nullptr;

//     --capacity_;

//     std::byte* tmp = first_;
//     first_ = get_next(first_);
//     return tmp;
// }

// void
// free_list::deallocate(void* addr) noexcept
// {
//     DEBUG_ASSERT(addr != nullptr);
//     ++capacity_;

//     std::byte* node = static_cast<decltype(node)>(addr);
//     set_next(node, first_);
//     first_ = node;
// }

// void
// free_list::set_next(void* addr, std::byte* ptr) noexcept
// {
//     set_int(addr, reinterpret_cast<std::uintptr_t>(ptr));
// }

// void
// free_list::set_int(void* addr, std::uintptr_t i) noexcept
// {
//     std::memcpy(addr, &i, sizeof(std::uintptr_t));
// }

// std::uintptr_t
// free_list::get_int(void* addr) noexcept
// {
//     DEBUG_ASSERT(addr != nullptr);
//     std::uintptr_t i = 0;
//     std::memcpy(&i, addr, sizeof(std::uintptr_t));
//     return i;
// }

// std::byte*
// free_list::get_next(void* addr) noexcept
// {
//     return reinterpret_cast<std::byte*>(get_int(addr));
// }
