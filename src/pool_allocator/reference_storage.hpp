#pragma once


class reference_storage_impl
{
protected:
    reference_storage_impl() noexcept
            : alloc_(nullptr)
    {}

    reference_storage_impl(memory_pool& allocator) noexcept
            : alloc_(&allocator)
    {}

    bool
    is_valid() const noexcept
    {
        return alloc_ != nullptr;
    }

    memory_pool&
    get_allocator() const noexcept
    {
        return *alloc_;
    }

private:
    memory_pool* alloc_;
};


class reference_storage
{
public:
    using allocator_type = memory_pool;
    /// Default constructor.
    /// \effects If the allocator is stateless, this has no effect and the object is usable as an
    /// allocator. If the allocator is stateful, creates an invalid reference without any associated
    /// allocator. Then it must not be used. If the allocator is shared, default constructs the
    /// shared allocator. If the shared allocator does not have a default constructor, this
    /// constructor is ill-formed.
    reference_storage() noexcept = default;

    /// \effects Creates it from a reference to a stateful allocator.
    /// It will store a pointer to this allocator object.
    /// \note The user has to take care that the lifetime of the reference does not exceed the
    /// allocator lifetime.
    reference_storage(memory_pool& alloc) noexcept
            : alloc_(&alloc)
    {}

    /// @{
    /// \effects Copies the \c allocator_reference object.
    /// Only copies the pointer to it in the stateful case.
    reference_storage(const reference_storage&) noexcept = default;
    reference_storage& operator=(const reference_storage&) noexcept = default;
    /// @}

    /// \returns Whether or not the reference is valid.
    /// It is only invalid, if it was created by the default constructor and the allocator is
    /// stateful.
    bool
    is_valid() const noexcept
    {
        return alloc_ != nullptr;
    }

    /// \returns Returns a reference to the allocator.
    /// \requires The reference must be valid.
    memory_pool&
    get_allocator() const noexcept
    {
        return *alloc_;
    }

    ~reference_storage() noexcept = default;

private:
    memory_pool* alloc_;
};
