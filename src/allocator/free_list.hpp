#pragma once

#include <cstddef> // std::size_t
#include <cstdint>


class free_list
{
public:
    static constexpr std::size_t min_element_size = sizeof(std::uint8_t*);
    static constexpr std::size_t min_element_alignment = alignof(std::uint8_t*);

    free_list(std::size_t node_size) noexcept;
    /// calls constructor plus insert
    free_list(std::size_t node_size, void* mem, std::size_t size) noexcept;
    free_list(free_list&& other) noexcept;
    ~free_list() noexcept = default;
    free_list& operator=(free_list&& other) noexcept;

    friend void swap(free_list& a, free_list& b) noexcept;

    void insert(void* mem, std::size_t size) noexcept;

    void* allocate() noexcept;
    void* allocate(std::size_t n) noexcept;
    void deallocate(void* ptr) noexcept;
    void deallocate(void* ptr, std::size_t n) noexcept;

    std::size_t node_size() const noexcept;
    std::size_t alignment() const noexcept;
    std::size_t capacity() const noexcept;
    bool empty() const noexcept;

private:
    void insert_impl(void* mem, std::size_t size) noexcept;

private:
    std::uint8_t* first_ = nullptr;
    std::size_t node_size_ = 0;
    std::size_t capacity_ = 0;
};
