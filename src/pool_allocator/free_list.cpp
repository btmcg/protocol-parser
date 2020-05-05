#include "free_list.hpp"
#include "common/assert.hpp"
#include <cstring> // std::memcpy


free_list::free_list(std::size_t node_size, void* addr, std::size_t size) noexcept
        : first_(nullptr)
        , node_size_(node_size > min_element_size ? node_size : min_element_size)
        , capacity_(0)
{
    std::size_t const actual_size = node_size_;
    std::size_t const num_nodes = size / actual_size;
    DEBUG_ASSERT(num_nodes > 0);

    std::byte* cur = static_cast<decltype(cur)>(addr);
    for (std::size_t i = 0; i < num_nodes; ++i) {
        set_next(cur, cur + actual_size);
        cur += actual_size;
    }
    set_next(cur, first_);
    first_ = static_cast<decltype(first_)>(addr);
    capacity_ = num_nodes;
}

free_list::free_list(free_list&& rhs) noexcept
        : first_(rhs.first_)
        , node_size_(rhs.node_size_)
        , capacity_(rhs.capacity_)
{
    rhs.first_ = nullptr;
    rhs.capacity_ = 0;
}

std::size_t
free_list::capacity() const noexcept
{
    return capacity_;
}

void*
free_list::allocate() noexcept
{
    if (capacity() == 0)
        return nullptr;

    --capacity_;

    std::byte* tmp = first_;
    first_ = get_next(first_);
    return tmp;
}

void
free_list::deallocate(void* addr) noexcept
{
    DEBUG_ASSERT(addr != nullptr);
    ++capacity_;

    std::byte* node = static_cast<decltype(node)>(addr);
    set_next(node, first_);
    first_ = node;
}

void
free_list::set_next(void* addr, std::byte* ptr) noexcept
{
    set_int(addr, reinterpret_cast<std::uintptr_t>(ptr));
}

void
free_list::set_int(void* addr, std::uintptr_t i) noexcept
{
    std::memcpy(addr, &i, sizeof(std::uintptr_t));
}

std::uintptr_t
free_list::get_int(void* addr) noexcept
{
    DEBUG_ASSERT(addr != nullptr);
    std::uintptr_t i = 0;
    std::memcpy(&i, addr, sizeof(std::uintptr_t));
    return i;
}

std::byte*
free_list::get_next(void* addr) noexcept
{
    return reinterpret_cast<std::byte*>(get_int(addr));
}
