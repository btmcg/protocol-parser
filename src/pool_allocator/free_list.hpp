#pragma once

#include <cstddef> // std::byte
#include <cstdint>


/// TODO/FIXME
class free_list
{
public:
    static constexpr std::size_t min_element_size = sizeof(std::uintptr_t);

public:
    free_list(std::size_t node_size, void* addr, std::size_t size) noexcept;
    ~free_list() noexcept = default;
    free_list(free_list&&) noexcept;

    std::size_t capacity() const noexcept; ///< num elements available to allocate

    void* allocate() noexcept;
    void deallocate(void*) noexcept;

private:
    void set_next(void*, std::byte*) noexcept;
    void set_int(void*, std::uintptr_t) noexcept;
    std::uintptr_t get_int(void*) noexcept;
    std::byte* get_next(void*) noexcept;

private:
    std::byte* first_;
    std::size_t node_size_;
    std::size_t capacity_;
};
