#pragma once

#include "allocator_traits.hpp"
#include "free_list.hpp"
#include "memory_arena.hpp"
#include "common/assert.hpp"
#include <cstdlib>
#include <new>
#include <type_traits>
#include <utility>


// class memory_arena
// {
// public:
//     memory_arena(std::size_t block_size)
//             : block_size_(block_size)
//     {}

//     memory_block
//     allocate_block()
//     {
//         void* mem = std::malloc(block_size_);
//         memory_block mb(mem, block_size_);
//         return mb;
//     }

// private:
//     std::size_t block_size_ = 0;
// };


// class memory_pool
// {
// public:
//     memory_pool(std::size_t node_size, std::size_t block_size)
//             : arena_(block_size)
//             , free_list_(node_size)
//     {
//         allocate_block();
//     }

//     void*
//     allocate_node()
//     {
//         if (free_list_.empty())
//             allocate_block();

//         DEBUG_ASSERT(!free_list_.empty());
//         return free_list_.allocate();
//     }

// private:
//     void
//     allocate_block()
//     {
//         memory_block mb = arena_.allocate_block();
//         free_list_.insert(mb.addr, mb.size);
//     }

// private:
//     memory_arena arena_;
//     free_list free_list_;
// };


/// A stateful \concept{concept_rawallocator,RawAllocator} that manages \concept{concept_node,nodes}
/// of fixed size. It uses a \ref memory_arena with a given \c BlockOrRawAllocator defaulting to
/// \ref growing_block_allocator, subdivides them in small nodes of given size and puts them onto a
/// free list. Allocation and deallocation simply remove or add nodes from this list and are thus
/// fast. The way the list is maintained can be controlled via the \c PoolType which is either \ref
/// node_pool, \ref array_pool or \ref small_node_pool.<br> This kind of allocator is ideal for
/// fixed size allocations and deallocations in any order, for example in a node based container
/// like \c std::list. It is not so good for different allocation sizes and has some drawbacks for
/// arrays as described in \ref memory_pool_type.hpp. \ingroup memory allocator
class memory_pool
{
public:
    using allocator_type = growing_block_allocator<>;

    static constexpr std::size_t min_node_size = free_list::min_element_size;

    /// \effects Creates it by specifying the size each \concept{concept_node,node} will have,
    /// the initial block size for the arena and other constructor arguments for the
    /// \concept{concept_blockallocator,BlockAllocator}. If the \c node_size is less than the \c
    /// min_node_size, the \c min_node_size will be the actual node size. It will allocate an
    /// initial memory block with given size from the
    /// \concept{concept_blockallocator,BlockAllocator} and puts it onto the free list. \requires \c
    /// node_size must be a valid \concept{concept_node,node size} and \c block_size must be a
    /// non-zero value.
    template <typename... Args>
    memory_pool(std::size_t node_size, std::size_t block_size, Args&&... args)
            : arena_(block_size, forward<Args>(args)...)
            , free_list_(node_size)
    {
        allocate_block();
    }

    /// \effects Destroys the \ref memory_pool by returning all memory blocks,
    /// regardless of properly deallocated back to the
    /// \concept{concept_blockallocator,BlockAllocator}.
    ~memory_pool() noexcept
    {}

    /// @{
    /// \effects Moving a \ref memory_pool object transfers ownership over the free list,
    /// i.e. the moved from pool is completely empty and the new one has all its memory.
    /// That means that it is not allowed to call \ref deallocate_node() on a moved-from allocator
    /// even when passing it memory that was previously allocated by this object.
    memory_pool(memory_pool&& other) noexcept
            : arena_(std::move(other.arena_))
            , free_list_(std::move(other.free_list_))
    {}

    memory_pool&
    operator=(memory_pool&& other) noexcept
    {
        arena_ = std::move(other.arena_);
        free_list_ = std::move(other.free_list_);
        return *this;
    }
    /// @}

    /// \effects Allocates a single \concept{concept_node,node} by removing it from the free list.
    /// If the free list is empty, a new memory block will be allocated from the arena and put onto
    /// it. The new block size will be \ref next_capacity() big. \returns A node of size \ref
    /// node_size() suitable aligned, i.e. suitable for any type where <tt>sizeof(T) <
    /// node_size()</tt>. \throws Anything thrown by the used
    /// \concept{concept_blockallocator,BlockAllocator}'s allocation function if a growth is needed.
    void*
    allocate_node()
    {
        if (free_list_.empty())
            allocate_block();
        DEBUG_ASSERT(!free_list_.empty());
        return free_list_.allocate();
    }

    /// \effects Allocates a single \concept{concept_node,node} similar to \ref allocate_node().
    /// But if the free list is empty, a new block will *not* be allocated.
    /// \returns A suitable aligned node of size \ref node_size() or `nullptr`.
    void*
    try_allocate_node() noexcept
    {
        return free_list_.empty() ? nullptr : free_list_.allocate();
    }

    /// \effects Allocates an \concept{concept_array,array} of nodes by searching for \c n
    /// continuous nodes on the list and removing them. Depending on the \c PoolType this can be a
    /// slow operation or not allowed at all. This can sometimes lead to a growth, even if
    /// technically there is enough continuous memory on the free list. \returns An array of \c n
    /// nodes of size \ref node_size() suitable aligned. \throws Anything thrown by the used
    /// \concept{concept_blockallocator,BlockAllocator}'s allocation function if a growth is needed,
    /// or \ref bad_array_size if <tt>n * node_size()</tt> is too big.
    /// \requires \c n must be valid \concept{concept_array,array count}.
    void*
    allocate_array(std::size_t n)
    {
        return allocate_array(n, node_size());
    }

    /// \effects Allocates an \concept{concept_array,array| of nodes similar to \ref
    /// allocate_array(). But it will never allocate a new memory block. \returns An array of \c n
    /// nodes of size \ref node_size() suitable aligned or `nullptr`.
    void*
    try_allocate_array(std::size_t n) noexcept
    {
        return try_allocate_array(n, node_size());
    }

    /// \effects Deallocates a single \concept{concept_node,node} by putting it back onto the free
    /// list. \requires \c ptr must be a result from a previous call to \ref allocate_node() on the
    /// same free list, i.e. either this allocator object or a new object created by moving this to
    /// it.
    void
    deallocate_node(void* ptr) noexcept
    {
        free_list_.deallocate(ptr);
    }

    /// \effects Deallocates a single \concept{concept_node,node} but it does not be a result of a
    /// previous call to \ref allocate_node(). \returns `true` if the node could be deallocated,
    /// `false` otherwise. \note Some free list implementations can deallocate any memory, doesn't
    /// matter where it is coming from.
    bool
    try_deallocate_node(void* ptr) noexcept
    {
        free_list_.deallocate(ptr);
        return true;
    }

    /// \effects Deallocates an \concept{concept_array,array} by putting it back onto the free list.
    /// \requires \c ptr must be a result from a previous call to \ref allocate_array() with the
    /// same \c n on the same free list, i.e. either this allocator object or a new object created
    /// by moving this to it.
    void
    deallocate_array(void* ptr, std::size_t n) noexcept
    {
        free_list_.deallocate(ptr, n * node_size());
    }

    /// \effects Deallocates an \concept{concept_array,array} but it does not be a result of a
    /// previous call to \ref allocate_array(). \returns `true` if the node could be deallocated,
    /// `false` otherwise. \note Some free list implementations can deallocate any memory, doesn't
    /// matter where it is coming from.
    bool
    try_deallocate_array(void* ptr, std::size_t n) noexcept
    {
        return try_deallocate_array(ptr, n, node_size());
    }

    /// \returns The size of each \concept{concept_node,node} in the pool,
    /// this is either the same value as in the constructor or \c min_node_size if the value was too
    /// small.
    std::size_t
    node_size() const noexcept
    {
        return free_list_.node_size();
    }

    /// \effects Returns the total amount of bytes remaining on the free list.
    /// Divide it by \ref node_size() to get the number of nodes that can be allocated without
    /// growing the arena. \note Array allocations may lead to a growth even if the capacity_left
    /// left is big enough.
    std::size_t
    capacity_left() const noexcept
    {
        return free_list_.capacity() * node_size();
    }

    /// \returns The size of the next memory block after the free list gets empty and the arena
    /// grows. \ref capacity_left() will increase by this amount. \note Due to fence memory in debug
    /// mode this cannot be just divided by the \ref node_size() to get the number of nodes.
    std::size_t
    next_capacity() const noexcept
    {
        return arena_.next_block_size();
    }

    /// \returns A reference to the \concept{concept_blockallocator,BlockAllocator} used for
    /// managing the arena. \requires It is undefined behavior to move this allocator out into
    /// another object.
    allocator_type&
    get_allocator() noexcept
    {
        return arena_.get_allocator();
    }

private:
    void
    allocate_block()
    {
        auto mem = arena_.allocate_block();
        free_list_.insert(static_cast<char*>(mem.memory), mem.size);
    }

    void*
    allocate_array(std::size_t n, std::size_t node_size)
    {
        auto mem = free_list_.empty() ? nullptr : free_list_.allocate(n * node_size);
        if (!mem) {
            allocate_block();
            mem = free_list_.allocate(n * node_size);
            if (!mem) {
                throw std::bad_alloc();
                // FOONATHAN_THROW(bad_array_size(info(), n * node_size, capacity_left()));
            }
        }
        return mem;
    }

    void*
    try_allocate_array(std::size_t n, std::size_t node_size) noexcept
    {
        return free_list_.empty() ? nullptr : free_list_.allocate(n * node_size);
    }

    bool
    try_deallocate_array(void* ptr, std::size_t n, std::size_t node_size) noexcept
    {
        free_list_.deallocate(ptr, n * node_size);
        return true;
    }

    memory_arena<growing_block_allocator<>> arena_;
    free_list free_list_;

    friend allocator_traits<memory_pool>;
};

constexpr std::size_t memory_pool::min_node_size;


/// Specialization of the \ref allocator_traits for \ref memory_pool classes.
/// \note It is not allowed to mix calls through the specialization and through the member
/// functions, i.e. \ref memory_pool::allocate_node() and this \c allocate_node(). \ingroup memory
/// allocator
template <>
class allocator_traits<memory_pool>
{
public:
    using allocator_type = memory_pool;
    using is_stateful = std::true_type;

    /// \returns The result of \ref memory_pool::allocate_node().
    /// \throws Anything thrown by the pool allocation function
    /// or a \ref bad_allocation_size exception.
    static void*
    allocate_node(allocator_type& state, std::size_t /*size*/, std::size_t /*alignment*/)
    {
        auto mem = state.allocate_node();
        return mem;
    }

    /// \effects Forwards to \ref memory_pool::allocate_array()
    /// with the number of nodes adjusted to be the minimum,
    /// i.e. when the \c size is less than the \ref memory_pool::node_size().
    /// \returns A \concept{concept_array,array} with specified properties.
    /// \requires The \ref memory_pool has to support array allocations.
    /// \throws Anything thrown by the pool allocation function.
    static void*
    allocate_array(
            allocator_type& state, std::size_t count, std::size_t size, std::size_t /*alignment*/)
    {
        auto mem = state.allocate_array(count, size);
        return mem;
    }

    /// \effects Just forwards to \ref memory_pool::deallocate_node().
    static void
    deallocate_node(allocator_type& state, void* node, std::size_t /*size*/, std::size_t) noexcept
    {
        state.deallocate_node(node);
    }

    /// \effects Forwards to \ref memory_pool::deallocate_array() with the same size adjustment.
    static void
    deallocate_array(allocator_type& state, void* array, std::size_t count, std::size_t size,
            std::size_t) noexcept
    {
        state.free_list_.deallocate(array, count * size);
    }

    /// \returns The maximum size of each node which is \ref memory_pool::node_size().
    static std::size_t
    max_node_size(const allocator_type& state) noexcept
    {
        return state.node_size();
    }

    /// \returns An upper bound on the maximum array size which is \ref
    /// memory_pool::next_capacity().
    static std::size_t
    max_array_size(const allocator_type& state) noexcept
    {
        return state.next_capacity();
    }

    /// \returns The maximum alignment which is the next bigger power of two if less than \c
    /// alignof(std::max_align_t) or the maximum alignment itself otherwise.
    static std::size_t
    max_alignment(const allocator_type& state) noexcept
    {
        return state.free_list_.alignment();
    }
};
