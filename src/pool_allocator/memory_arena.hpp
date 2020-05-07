#pragma once

#include "allocator_traits.hpp"
#include "heap_allocator.hpp"
#include "common/assert.hpp"
#include <utility>


/// A memory block.
/// It is defined by its starting address and size.
/// \ingroup memory core
struct memory_block
{
    void* memory; ///< The address of the memory block (might be \c nullptr).
    std::size_t size; ///< The size of the memory block (might be \c 0).

    /// \effects Creates an invalid memory block with starting address \c nullptr and size \c 0.
    memory_block() noexcept
            : memory_block(nullptr, std::size_t(0))
    {}

    /// \effects Creates a memory block from a given starting address and size.
    memory_block(void* mem, std::size_t s) noexcept
            : memory(mem)
            , size(s)
    {}

    /// \effects Creates a memory block from a [begin,end) range.
    memory_block(void* begin, void* end) noexcept
            : memory_block(begin,
                    static_cast<std::size_t>(static_cast<char*>(end) - static_cast<char*>(begin)))
    {}

    /// \returns Whether or not a pointer is inside the memory.
    bool
    contains(const void* address) const noexcept
    {
        auto mem = static_cast<const char*>(memory);
        auto addr = static_cast<const char*>(address);
        return addr >= mem && addr < mem + size;
    }
};


/// A \concept{concept_blockallocator,BlockAllocator} that uses a given
/// \concept{concept_rawallocator,RawAllocator} for allocating the blocks. It calls the \c
/// allocate_array() function with a node of size \c 1 and maximum alignment on the used allocator
/// for the block allocation. The size of the next memory block will grow by a given factor after
/// each allocation, allowing an amortized constant allocation time in the higher level allocator.
/// The factor can be given as rational in the template parameter, default is \c 2.
/// \ingroup memory adapter
template <class RawAllocator = heap_allocator, unsigned Num = 2, unsigned Den = 1>
class growing_block_allocator : allocator_traits<RawAllocator>::allocator_type
{
    static_assert(float(Num) / Den >= 1.0, "invalid growth factor");

    using traits = allocator_traits<RawAllocator>;

public:
    using allocator_type = typename traits::allocator_type;

    /// \effects Creates it by giving it the initial block size, the allocator object and the growth
    /// factor. By default, it uses a default-constructed allocator object and a growth factor of
    /// \c 2. \requires \c block_size must be greater than 0.
    explicit growing_block_allocator(
            std::size_t block_size, allocator_type alloc = allocator_type()) noexcept
            : allocator_type(std::move(alloc))
            , block_size_(block_size)
    {}

    /// \effects Allocates a new memory block and increases the block size for the next allocation.
    /// \returns The new \ref memory_block.
    /// \throws Anything thrown by the \c allocate_array() function of the
    /// \concept{concept_rawallocator,RawAllocator}.
    memory_block
    allocate_block()
    {
        auto memory = traits::allocate_array(
                get_allocator(), block_size_, 1, alignof(std::max_align_t));
        memory_block block(memory, block_size_);
        block_size_ = std::size_t(block_size_ * growth_factor());
        return block;
    }

    /// \effects Deallocates a previously allocated memory block.
    /// This does not decrease the block size.
    /// \requires \c block must be previously returned by a call to \ref allocate_block().
    void
    deallocate_block(memory_block block) noexcept
    {
        traits::deallocate_array(
                get_allocator(), block.memory, block.size, 1, alignof(std::max_align_t));
    }

    /// \returns The size of the memory block returned by the next call to \ref allocate_block().
    std::size_t
    next_block_size() const noexcept
    {
        return block_size_;
    }

    /// \returns A reference to the used \concept{concept_rawallocator,RawAllocator} object.
    allocator_type&
    get_allocator() noexcept
    {
        return *this;
    }

    /// \returns The growth factor.
    static float
    growth_factor() noexcept
    {
        static constexpr auto factor = float(Num) / Den;
        return factor;
    }

private:
    std::size_t block_size_;
};


// stores memory block in an intrusive linked list and allows LIFO access
class memory_block_stack
{
public:
    memory_block_stack() noexcept
            : head_(nullptr)
    {}

    ~memory_block_stack() noexcept
    {}

    memory_block_stack(memory_block_stack&& other) noexcept
            : head_(other.head_)
    {
        other.head_ = nullptr;
    }

    memory_block_stack&
    operator=(memory_block_stack&& other) noexcept
    {
        memory_block_stack tmp(std::move(other));
        swap(*this, tmp);
        return *this;
    }

    friend void
    swap(memory_block_stack& a, memory_block_stack& b) noexcept
    {
        std::swap(a.head_, b.head_);
    }

    // the raw allocated block returned from an allocator
    using allocated_mb = memory_block;

    // the inserted block slightly smaller to allow for the fixup value
    using inserted_mb = memory_block;

    // how much an inserted block is smaller
    static const std::size_t implementation_offset;

    // pushes a memory block
    void
    push(allocated_mb block) noexcept
    {
        auto next = ::new (block.memory) node(head_, block.size - node::offset);
        head_ = next;
    }

    // pops a memory block and returns the original block
    allocated_mb
    pop() noexcept
    {
        DEBUG_ASSERT(head_);
        auto to_pop = head_;
        head_ = head_->prev;
        return {to_pop, to_pop->usable_size + node::offset};
    }

    // steals the top block from another stack
    void
    steal_top(memory_block_stack& other) noexcept
    {
        DEBUG_ASSERT(other.head_);
        auto to_steal = other.head_;
        other.head_ = other.head_->prev;

        to_steal->prev = head_;
        head_ = to_steal;
    }

    // returns the last pushed() inserted memory block
    inserted_mb
    top() const noexcept
    {
        DEBUG_ASSERT(head_);
        auto mem = static_cast<void*>(head_);
        return {static_cast<char*>(mem) + node::offset, head_->usable_size};
    }

    bool
    empty() const noexcept
    {
        return head_ == nullptr;
    }

    bool owns(const void* ptr) const noexcept;

    // O(n) size
    std::size_t size() const noexcept;

private:
    struct node
    {
        node* prev;
        std::size_t usable_size;

        node(node* p, std::size_t size) noexcept
                : prev(p)
                , usable_size(size)
        {}

        static const std::size_t div_alignment;
        static const std::size_t mod_offset;
        static const std::size_t offset;
    };

    node* head_;
};

constexpr std::size_t max_alignment = alignof(std::max_align_t);

const std::size_t memory_block_stack::node::div_alignment
        = sizeof(memory_block_stack::node) / max_alignment;
const std::size_t memory_block_stack::node::mod_offset
        = sizeof(memory_block_stack::node) % max_alignment != 0u;
const std::size_t memory_block_stack::node::offset = (div_alignment + mod_offset) * max_alignment;

const std::size_t memory_block_stack::implementation_offset = memory_block_stack::node::offset;


/// A memory arena that manages huge memory blocks for a higher-level allocator.
/// Some allocators like \ref memory_stack work on huge memory blocks,
/// this class manages them fro those allocators.
/// It uses a \concept{concept_blockallocator,BlockAllocator} for the allocation of those blocks.
/// The memory blocks in use are put onto a stack like structure, deallocation will pop from the
/// top, so it is only possible to deallocate the last allocated block of the arena. By default,
/// blocks are not really deallocated but stored in a cache. This can be disabled with the second
/// template parameter, passing it \ref uncached_arena (or \c false) disables it, \ref cached_arena
/// (or \c true) enables it explicitly. \ingroup memory core
template <class BlockAllocator>
class memory_arena : BlockAllocator
{
public:
    using allocator_type = BlockAllocator;

    /// \effects Creates it by giving it the size and other arguments for the
    /// \concept{concept_blockallocator,BlockAllocator}. It forwards these arguments to its
    /// constructor. \requires \c block_size must be greater than \c 0 and other requirements
    /// depending on the \concept{concept_blockallocator,BlockAllocator}. \throws Anything thrown by
    /// the constructor of the \c BlockAllocator.
    template <typename... Args>
    explicit memory_arena(std::size_t block_size, Args&&... args)
            : allocator_type(block_size, forward<Args>(args)...)
    {}

    /// \effects Deallocates all memory blocks that where requested back to the
    /// \concept{concept_blockallocator,BlockAllocator}.
    ~memory_arena() noexcept
    {
        // clear cache
        shrink_to_fit();
        // now deallocate everything
        while (!used_.empty())
            allocator_type::deallocate_block(used_.pop());
    }

    /// @{
    /// \effects Moves the arena.
    /// The new arena takes ownership over all the memory blocks from the other arena object,
    /// which is empty after that.
    /// This does not invalidate any memory blocks.
    memory_arena(memory_arena&& other) noexcept
            : allocator_type(std::move(other))
            , used_(std::move(other.used_))
    {}

    memory_arena&
    operator=(memory_arena&& other) noexcept
    {
        memory_arena tmp(std::move(other));
        swap(*this, tmp);
        return *this;
    }
    /// @}

    /// \effects Swaps to memory arena objects.
    /// This does not invalidate any memory blocks.
    friend void
    swap(memory_arena& a, memory_arena& b) noexcept
    {
        std::swap(static_cast<allocator_type&>(a), static_cast<allocator_type&>(b));
        std::swap(a.used_, b.used_);
    }

    /// \effects Allocates a new memory block.
    /// It first uses a cache of previously deallocated blocks, if caching is enabled,
    /// if it is empty, allocates a new one.
    /// \returns The new \ref memory_block.
    /// \throws Anything thrown by the \concept{concept_blockallocator,BlockAllocator} allocation
    /// function.
    memory_block
    allocate_block()
    {
        used_.push(allocator_type::allocate_block());

        auto block = used_.top();
        return block;
    }

    /// \returns The current memory block.
    /// This is the memory block that will be deallocated by the next call to \ref
    /// deallocate_block().
    memory_block
    current_block() const noexcept
    {
        return used_.top();
    }

    /// \effects Deallocates the current memory block.
    /// The current memory block is the block on top of the stack of blocks.
    /// If caching is enabled, it does not really deallocate it but puts it onto a cache for later
    /// use, use \ref shrink_to_fit() to purge that cache.
    void
    deallocate_block() noexcept
    {
        // auto block = used_.top();
        this->do_deallocate_block(get_allocator(), used_);
    }

    /// \returns If `ptr` is in memory owned by the arena.
    bool
    owns(const void* ptr) const noexcept
    {
        return used_.owns(ptr);
    }

    /// \effects Purges the cache of unused memory blocks by returning them.
    /// The memory blocks will be deallocated in reversed order of allocation.
    /// Does nothing if caching is disabled.
    void
    shrink_to_fit() noexcept
    {}

    /// \returns The capacity of the arena, i.e. how many blocks are used and cached.
    std::size_t
    capacity() const noexcept
    {
        return size();
    }

    /// \returns The size of the arena, i.e. how many blocks are in use.
    /// It is always smaller or equal to the \ref capacity().
    std::size_t
    size() const noexcept
    {
        return used_.size();
    }

    /// \returns The size of the next memory block,
    /// i.e. of the next call to \ref allocate_block().
    /// If there are blocks in the cache, returns size of the next one.
    /// Otherwise forwards to the \concept{concept_blockallocator,BlockAllocator} and subtracts an
    /// implementation offset.
    std::size_t
    next_block_size() const noexcept
    {
        return allocator_type::next_block_size() - memory_block_stack::implementation_offset;
    }

    /// \returns A reference of the \concept{concept_blockallocator,BlockAllocator} object.
    /// \requires It is undefined behavior to move this allocator out into another object.
    allocator_type&
    get_allocator() noexcept
    {
        return *this;
    }

private:
    memory_block_stack used_;
};
