#pragma once

#include <fmt/format.h>
#include <cstddef> // std::byte
#include <cstdint>
#include <cstdlib>
#include <utility> // std::forward


template <typename T, std::size_t NumElements = 10>
class pool_allocator
{
public:
    using value_type = T;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    template <typename U>
    struct rebind
    {
        using other = pool_allocator<T>;
    };

public:
    constexpr pool_allocator() noexcept;
    constexpr pool_allocator(pool_allocator const&) noexcept = delete;
    template <typename U, std::size_t NumU>
    constexpr pool_allocator(pool_allocator<U, NumU> const&) noexcept = delete;
    ~pool_allocator() noexcept;
    [[nodiscard]] constexpr value_type* allocate(size_type) noexcept;
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

    std::byte* block_begin_;
    std::byte* block_end_;
    node* curr_node_;
    node* last_node_;
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
    fmt::print("pool_allocator::{}()\n", __func__);
    allocate_block();
}

// template <typename T, std::size_t NumElements>
// constexpr pool_allocator<T, NumElements>::pool_allocator(pool_allocator const& rhs) noexcept
//         : block_begin_(rhs.block_begin_)
//         , block_end_(rhs.block_end_)
//         , curr_node_(rhs.curr_node_)
//         , last_node_(rhs.last_node_)
// {
//     fmt::print("pool_allocator::{}()\n", __func__);
//     // empty
// }

// template <typename T, std::size_t NumElements>
// template <typename U, std::size_t NumU>
// constexpr pool_allocator<T, NumElements>::pool_allocator(
//         pool_allocator<U, NumU> const& rhs) noexcept
//         : block_begin_(rhs.block_begin_)
//         , block_end_(rhs.block_end_)
//         , curr_node_(rhs.curr_node_)
//         , last_node_(rhs.last_node_)
// {
//     fmt::print("pool_allocator::{}()\n", __func__);
//     // empty
// }

template <typename T, std::size_t NumElements>
pool_allocator<T, NumElements>::~pool_allocator() noexcept
{
    fmt::print("pool_allocator::{}()\n", __func__);
    operator delete(block_begin_);
    block_begin_ = nullptr;
}

template <typename T, std::size_t NumElements>
constexpr typename pool_allocator<T, NumElements>::value_type*
        pool_allocator<T, NumElements>::allocate(pool_allocator<T, NumElements>::size_type) noexcept
{
    fmt::print("pool_allocator::{}()\n", __func__);

    if (curr_node_ == nullptr) {
        fmt::print(stderr, "ERROR: pool_allocator::{}() out of space\n", __func__);
        return nullptr;
    }

    value_type* ptr = (&curr_node_->element);
    curr_node_ = curr_node_->next;

    fmt::print(
            "pool_allocator::{}() ptr={} curr_node_={}\n", __func__, (void*)ptr, (void*)curr_node_);
    return ptr;
}

template <typename T, std::size_t NumElements>
constexpr void
pool_allocator<T, NumElements>::deallocate(
        pool_allocator<T, NumElements>::value_type* ptr, pool_allocator<T, NumElements>::size_type)
{
    fmt::print("pool_allocator::{}()\n", __func__);
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
    fmt::print("pool_allocator::{}()\n", __func__);
    ::new (ptr) U(std::forward<Args>(args)...);
}

template <typename T, std::size_t NumElements>
template <typename U>
void
pool_allocator<T, NumElements>::destroy(U* ptr)
{
    fmt::print("pool_allocator::{}()\n", __func__);
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
    fmt::print("pool_allocator::{}()\n", __func__);
    if (block_begin_ != nullptr) {
        fmt::print(stderr, "pool_allocator::{}() block_begin_ != nullptr\n", __func__);
        std::exit(1);
    }

    fmt::print("pool_allocator::{}() node size = {} bytes\n", __func__, sizeof(node));
    fmt::print("pool_allocator::{}() allocating {} bytes\n", __func__, NumElements * sizeof(node));
    block_begin_
            = reinterpret_cast<decltype(block_begin_)>(operator new(NumElements * sizeof(node)));
    block_end_ = block_begin_ + (NumElements * sizeof(node));
    curr_node_ = reinterpret_cast<decltype(curr_node_)>(block_begin_);

    fmt::print("pool_allocator::{}() block_begin_={}\n", __func__, (void*)block_begin_);
    fmt::print("pool_allocator::{}() block_end_={}\n", __func__, (void*)block_end_);
    fmt::print("pool_allocator::{}() curr_node_={}\n", __func__, (void*)curr_node_);
    node* tmp = curr_node_;
    for (std::size_t i = 1; i < NumElements; ++i) {
        tmp->next = tmp + 1;
        tmp = tmp->next;
        fmt::print("pool_allocator::{}() node {}={}\n", __func__, i, (void*)tmp);
    }
    last_node_ = tmp;
    last_node_->next = nullptr;
    fmt::print("pool_allocator::{}() last_node_={}\n", __func__, (void*)last_node_);
}

/**********************************************************************/

template <typename T, std::size_t NumElements, typename U, std::size_t NumU>
constexpr bool
operator==(pool_allocator<T, NumElements> const&, pool_allocator<U, NumU> const&) noexcept
{
    fmt::print("pool_allocator::{}()\n", __func__);
    return false;
}

template <typename T, std::size_t NumElements, typename U, std::size_t NumU>
constexpr bool
operator!=(pool_allocator<T, NumElements> const&, pool_allocator<U, NumU> const&) noexcept
{
    fmt::print("pool_allocator::{}()\n", __func__);
    return true;
}
