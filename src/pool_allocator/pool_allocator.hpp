#pragma once

#include <fmt/format.h>
#include <cstddef> // std::byte
#include <cstdlib>
#include <new>
#include <utility> // std::forward


template <typename T, std::size_t NumElements>
class pool_allocator
{
public:
    using value_type = T;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    template <typename U>
    struct rebind
    {
        using other = pool_allocator<U, NumElements>;
    };

public:
    constexpr pool_allocator() noexcept;
    constexpr pool_allocator(pool_allocator const&) noexcept = delete;
    template <typename U, std::size_t NumU>
    constexpr pool_allocator(pool_allocator<U, NumU> const&) noexcept = delete;
    ~pool_allocator() noexcept;
    [[nodiscard]] constexpr value_type* allocate(size_type);
    constexpr void deallocate(value_type*, size_type);
    template <typename U, class... Args>
    void construct(U*, Args&&...);
    template <typename U>
    void destroy(U*);
    constexpr std::size_t capacity() const noexcept;

private:
    void allocate_block();

private:
    union node
    {
        value_type element;
        node* next;
    };

    std::byte* block_begin_ = nullptr;
    std::byte* block_end_ = nullptr;
    node* curr_node_ = nullptr;
    node* last_node_ = nullptr;
};


template <typename T, std::size_t NumElements, typename U, std::size_t NumU>
constexpr bool operator==(
        pool_allocator<T, NumElements> const&, pool_allocator<U, NumU> const&) noexcept;

template <typename T, std::size_t NumElements, typename U, std::size_t NumU>
constexpr bool operator!=(
        pool_allocator<T, NumElements> const&, pool_allocator<U, NumU> const&) noexcept;

/**********************************************************************/

template <typename T, std::size_t NumElements>
constexpr pool_allocator<T, NumElements>::pool_allocator() noexcept
        : block_begin_(nullptr)
        , block_end_(nullptr)
        , curr_node_(nullptr)
        , last_node_(nullptr)
{
    allocate_block();
}

template <typename T, std::size_t NumElements>
pool_allocator<T, NumElements>::~pool_allocator() noexcept
{
    operator delete(block_begin_);
    block_begin_ = nullptr;
}

template <typename T, std::size_t NumElements>
constexpr typename pool_allocator<T, NumElements>::value_type*
        pool_allocator<T, NumElements>::allocate(pool_allocator<T, NumElements>::size_type)
{
    if (curr_node_ == nullptr)
        throw std::bad_alloc();

    value_type* ptr = (&curr_node_->element);
    curr_node_ = curr_node_->next;
    return ptr;
}

template <typename T, std::size_t NumElements>
constexpr void
pool_allocator<T, NumElements>::deallocate(
        pool_allocator<T, NumElements>::value_type* ptr, pool_allocator<T, NumElements>::size_type)
{
    if (ptr == nullptr)
        return;
    reinterpret_cast<node*>(ptr)->next = curr_node_;
    curr_node_ = reinterpret_cast<node*>(ptr);
}

template <typename T, std::size_t NumElements>
template <typename U, typename... Args>
void
pool_allocator<T, NumElements>::construct(U* ptr, Args&&... args)
{
    ::new (ptr) U(std::forward<Args>(args)...);
}

template <typename T, std::size_t NumElements>
template <typename U>
void
pool_allocator<T, NumElements>::destroy(U* ptr)
{
    ptr->~U();
}

template <typename T, std::size_t NumElements>
constexpr std::size_t
pool_allocator<T, NumElements>::capacity() const noexcept
{
    return std::distance(block_begin_, block_end_);
}

template <typename T, std::size_t NumElements>
void
pool_allocator<T, NumElements>::allocate_block()
{
    if (block_begin_ != nullptr) {
        fmt::print(stderr, "pool_allocator::{}() block_begin_ != nullptr\n", __func__);
        std::exit(1);
    }

    block_begin_ = reinterpret_cast<std::byte*>(operator new(NumElements * sizeof(node)));
    block_end_ = block_begin_ + (NumElements * sizeof(node));
    curr_node_ = reinterpret_cast<decltype(curr_node_)>(block_begin_);

    node* tmp = curr_node_;
    for (std::size_t i = 1; i < NumElements; ++i) {
        tmp->next = tmp + 1;
        tmp = tmp->next;
    }
    last_node_ = tmp;
    last_node_->next = nullptr;
}

/**********************************************************************/

template <typename T, std::size_t NumElements, typename U, std::size_t NumU>
constexpr bool
operator==(pool_allocator<T, NumElements> const&, pool_allocator<U, NumU> const&) noexcept
{
    return false;
}

template <typename T, std::size_t NumElements, typename U, std::size_t NumU>
constexpr bool
operator!=(pool_allocator<T, NumElements> const&, pool_allocator<U, NumU> const&) noexcept
{
    return true;
}
