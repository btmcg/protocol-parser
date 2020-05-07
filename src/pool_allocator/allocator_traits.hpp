#pragma once

#include "common/assert.hpp"
#include <cstddef> // std::max_align_t
#include <type_traits>


// same as above, but requires certain type
#define FOONATHAN_AUTO_RETURN_TYPE(Expr, T)                   \
    decltype(Expr)                                            \
    {                                                         \
        static_assert(std::is_same<decltype(Expr), T>::value, \
                #Expr " does not have the return type " #T);  \
        return Expr;                                          \
    }

// avoids code repetition for one-line forwarding functions
#define FOONATHAN_AUTO_RETURN(Expr) \
    decltype(Expr)                  \
    {                               \
        return Expr;                \
    }

namespace detail {
    constexpr std::size_t max_alignment = alignof(std::max_align_t);
}

// namespace traits_detail // use seperate namespace to avoid name clashes
//{
//    // full_concept has the best conversion rank, error the lowest
//    // used to give priority to the functions
//    struct error
//    {
//        operator void*() const noexcept
//        {
//            return nullptr;
//        }
//    };
//    struct std_concept : error
//    {};
//    struct min_concept : std_concept
//    {};
//    struct full_concept : min_concept
//    {};

//    // used to delay assert in handle_error() until instantiation
//    template <typename T>
//    struct invalid_allocator_concept
//    {
//        static const bool error = false;
//    };

//    //=== allocator_type ===//
//    // if Allocator has a member template `rebind`, use that to rebind to `char`
//    // else if Allocator has a member `value_type`, rebind by changing argument
//    // else does nothing
//    template <class Allocator>
//    auto rebind_impl(int) -> typename Allocator::template rebind<char>::other&;

//    template <class Allocator, typename T>
//    struct allocator_rebinder
//    {
//        using type = Allocator&;
//    };

//    template <template <typename, typename...> class Alloc, typename U, typename... Args,
//            typename T>
//    struct allocator_rebinder<Alloc<U, Args...>, T>
//    {
//        using type = Alloc<T, Args...>&;
//    };

//    template <class Allocator, typename = typename Allocator::value_type>
//    auto rebind_impl(char) -> typename allocator_rebinder<Allocator, char>::type;

//    template <class Allocator>
//    auto rebind_impl(...) -> Allocator&;

//    template <class Allocator>
//    struct allocator_type_impl // required for MSVC
//    {
//        using type = decltype(rebind_impl<Allocator>(0));
//    };

//    template <class Allocator>
//    using allocator_type = typename std::decay<typename
//    allocator_type_impl<Allocator>::type>::type;

//    //=== is_stateful ===//
//    // first try to access Allocator::is_stateful,
//    // then use whether or not the type is empty
//    template <class Allocator>
//    auto is_stateful(full_concept) -> decltype(typename std::true_type{});

//    template <class Allocator, bool IsEmpty>
//    struct is_stateful_impl;

//    template <class Allocator>
//    struct is_stateful_impl<Allocator, true>
//    {
//        static_assert(std::is_default_constructible<Allocator>::value,
//                "RawAllocator is empty but not default constructible ."
//                "This means it is not a stateless allocator. "
//                "If this is actually intended provide the appropriate is_stateful "
//                "typedef in your class.");
//        using type = std::false_type;
//    };

//    template <class Allocator>
//    struct is_stateful_impl<Allocator, false>
//    {
//        using type = std::true_type;
//    };

//    template <class Allocator>
//    auto is_stateful(min_concept) ->
//            typename is_stateful_impl<Allocator, std::is_empty<Allocator>::value>::type;

//    //=== allocate_node() ===//
//    // first try Allocator::allocate_node
//    // then assume std_allocator and call Allocator::allocate
//    // then error
//    template <class Allocator>
//    void*
//    allocate_node(Allocator& alloc, std::size_t size, std::size_t alignment)
//    {
//        return alloc.allocate_node(size, alignment);
//    }

//    //=== deallocate_node() ===//
//    // first try Allocator::deallocate_node
//    // then assume std_allocator and call Allocator::deallocate
//    // then error
//    template <class Allocator>
//    void
//    deallocate_node(Allocator& alloc, void* ptr, std::size_t size, std::size_t alignment) noexcept
//    {
//        alloc.deallocate_node(ptr, size, alignment);
//    }

//    //=== allocate_array() ===//
//    // first try Allocator::allocate_array
//    // then forward to allocate_node()
//    template <class Allocator>
//    void*
//    allocate_array(Allocator& alloc, std::size_t count, std::size_t size, std::size_t alignment)
//    {
//        return alloc.allocate_array(count, size, alignment);
//    }

//    //=== deallocate_array() ===//
//    // first try Allocator::deallocate_array
//    // then forward to deallocate_node()
//    template <class Allocator>
//    void
//    deallocate_array(Allocator& alloc, void* ptr, std::size_t count, std::size_t size,
//            std::size_t alignment) noexcept
//    {
//        deallocate_node(alloc, ptr, count * size, alignment);
//    }

//    //=== max_node_size() ===//
//    // first try Allocator::max_node_size()
//    // then return maximum value
//    template <class Allocator>
//    std::size_t
//    max_node_size(const Allocator& alloc)
//    {
//        return alloc.max_node_size();
//    }

//    //=== max_node_size() ===//
//    // first try Allocator::max_array_size()
//    // then forward to max_node_size()
//    template <class Allocator>
//    std::size_t
//    max_array_size(const Allocator& alloc)
//    {
//        return alloc.max_array_size();
//    }

//    //=== max_alignment() ===//
//    // first try Allocator::max_alignment()
//    // then return detail::max_alignment
//    template <class Allocator>
//    std::size_t
//    max_alignment(const Allocator& alloc)
//    {
//        return alloc.max_alignment();
//    }
//} // namespace traits_detail


namespace traits_detail // use seperate namespace to avoid name clashes
{
    // full_concept has the best conversion rank, error the lowest
    // used to give priority to the functions
    struct error
    {
        operator void*() const noexcept
        {
            DEBUG_ASSERT(false);
            return nullptr;
        }
    };
    struct std_concept : error
    {};
    struct min_concept : std_concept
    {};
    struct full_concept : min_concept
    {};

    // used to delay assert in handle_error() until instantiation
    template <typename T>
    struct invalid_allocator_concept
    {
        static const bool error = false;
    };

    //=== allocator_type ===//
    // if Allocator has a member template `rebind`, use that to rebind to `char`
    // else if Allocator has a member `value_type`, rebind by changing argument
    // else does nothing
    template <class Allocator>
    auto rebind_impl(int) -> typename Allocator::template rebind<char>::other&;

    template <class Allocator, typename T>
    struct allocator_rebinder
    {
        using type = Allocator&;
    };

    template <template <typename, typename...> class Alloc, typename U, typename... Args,
            typename T>
    struct allocator_rebinder<Alloc<U, Args...>, T>
    {
        using type = Alloc<T, Args...>&;
    };

    template <class Allocator, typename = typename Allocator::value_type>
    auto rebind_impl(char) -> typename allocator_rebinder<Allocator, char>::type;

    template <class Allocator>
    auto rebind_impl(...) -> Allocator&;

    template <class Allocator>
    struct allocator_type_impl // required for MSVC
    {
        using type = decltype(rebind_impl<Allocator>(0));
    };

    template <class Allocator>
    using allocator_type = typename std::decay<typename allocator_type_impl<Allocator>::type>::type;

    //=== is_stateful ===//
    // first try to access Allocator::is_stateful,
    // then use whether or not the type is empty
    template <class Allocator>
    auto is_stateful(full_concept) -> decltype(typename Allocator::is_stateful{});

    template <class Allocator, bool IsEmpty>
    struct is_stateful_impl;

    template <class Allocator>
    struct is_stateful_impl<Allocator, true>
    {
        static_assert(std::is_default_constructible<Allocator>::value,
                "RawAllocator is empty but not default constructible ."
                "This means it is not a stateless allocator. "
                "If this is actually intended provide the appropriate is_stateful "
                "typedef in your class.");
        using type = std::false_type;
    };

    template <class Allocator>
    struct is_stateful_impl<Allocator, false>
    {
        using type = std::true_type;
    };

    template <class Allocator>
    auto is_stateful(min_concept) ->
            typename is_stateful_impl<Allocator, std::is_empty<Allocator>::value>::type;

    //=== allocate_node() ===//
    // first try Allocator::allocate_node
    // then assume std_allocator and call Allocator::allocate
    // then error
    template <class Allocator>
    auto
    allocate_node(full_concept, Allocator& alloc, std::size_t size, std::size_t alignment)
            -> FOONATHAN_AUTO_RETURN_TYPE(alloc.allocate_node(size, alignment), void*)

                    template <class Allocator>
                    auto allocate_node(std_concept, Allocator& alloc, std::size_t size, std::size_t)
                            -> FOONATHAN_AUTO_RETURN(static_cast<void*>(alloc.allocate(size)))

                                    template <class Allocator>
                                    error allocate_node(error, Allocator&, std::size_t, std::size_t)
    {
        static_assert(invalid_allocator_concept<Allocator>::error,
                "type is not a RawAllocator as it does not provide: void* "
                "allocate_node(std::size_t, "
                "std::size_t)");
        return {};
    }

    //=== deallocate_node() ===//
    // first try Allocator::deallocate_node
    // then assume std_allocator and call Allocator::deallocate
    // then error
    template <class Allocator>
    auto
    deallocate_node(full_concept, Allocator& alloc, void* ptr, std::size_t size,
            std::size_t alignment) noexcept
            -> FOONATHAN_AUTO_RETURN_TYPE(alloc.deallocate_node(ptr, size, alignment), void)

                    template <class Allocator>
                    auto deallocate_node(std_concept, Allocator& alloc, void* ptr, std::size_t size,
                            std::size_t) noexcept
            -> FOONATHAN_AUTO_RETURN_TYPE(alloc.deallocate(static_cast<char*>(ptr), size), void)

                    template <class Allocator>
                    error deallocate_node(error, Allocator&, void*, std::size_t, std::size_t)
    {
        static_assert(invalid_allocator_concept<Allocator>::error,
                "type is not a RawAllocator as it does not provide: void "
                "deallocate_node(void*, std::size_t, "
                "std::size_t)");
        return error{};
    }

    //=== allocate_array() ===//
    // first try Allocator::allocate_array
    // then forward to allocate_node()
    template <class Allocator>
    auto
    allocate_array(full_concept, Allocator& alloc, std::size_t count, std::size_t size,
            std::size_t alignment)
            -> FOONATHAN_AUTO_RETURN_TYPE(alloc.allocate_array(count, size, alignment), void*)

                    template <class Allocator>
                    void* allocate_array(min_concept, Allocator& alloc, std::size_t count,
                            std::size_t size, std::size_t alignment)
    {
        return allocate_node(full_concept{}, alloc, count * size, alignment);
    }

    //=== deallocate_array() ===//
    // first try Allocator::deallocate_array
    // then forward to deallocate_node()
    template <class Allocator>
    auto
    deallocate_array(full_concept, Allocator& alloc, void* ptr, std::size_t count, std::size_t size,
            std::size_t alignment) noexcept
            -> FOONATHAN_AUTO_RETURN_TYPE(alloc.deallocate_array(ptr, count, size, alignment), void)

                    template <class Allocator>
                    void deallocate_array(min_concept, Allocator& alloc, void* ptr,
                            std::size_t count, std::size_t size, std::size_t alignment) noexcept
    {
        deallocate_node(full_concept{}, alloc, ptr, count * size, alignment);
    }

    //=== max_node_size() ===//
    // first try Allocator::max_node_size()
    // then return maximum value
    template <class Allocator>
    auto
    max_node_size(full_concept, const Allocator& alloc)
            -> FOONATHAN_AUTO_RETURN_TYPE(alloc.max_node_size(), std::size_t)

                    template <class Allocator>
                    std::size_t max_node_size(min_concept, const Allocator&) noexcept
    {
        return std::size_t(-1);
    }

    //=== max_node_size() ===//
    // first try Allocator::max_array_size()
    // then forward to max_node_size()
    template <class Allocator>
    auto
    max_array_size(full_concept, const Allocator& alloc)
            -> FOONATHAN_AUTO_RETURN_TYPE(alloc.max_array_size(), std::size_t)

                    template <class Allocator>
                    std::size_t max_array_size(min_concept, const Allocator& alloc)
    {
        return max_node_size(full_concept{}, alloc);
    }

    //=== max_alignment() ===//
    // first try Allocator::max_alignment()
    // then return detail::max_alignment
    template <class Allocator>
    auto
    max_alignment(full_concept, const Allocator& alloc)
            -> FOONATHAN_AUTO_RETURN_TYPE(alloc.max_alignment(), std::size_t)

                    template <class Allocator>
                    std::size_t max_alignment(min_concept, const Allocator&)
    {
        return detail::max_alignment;
    }
} // namespace traits_detail


/// The default specialization of the allocator_traits for a
/// \concept{concept_rawallocator,RawAllocator}. See the last link for the requirements on types
/// that do not specialize this class and the interface documentation. Any specialization must
/// provide the same interface. \ingroup memory core
template <class Allocator>
class allocator_traits
{
public:
    using allocator_type = traits_detail::allocator_type<Allocator>;
    using is_stateful
            = decltype(traits_detail::is_stateful<Allocator>(traits_detail::full_concept{}));

    static void*
    allocate_node(allocator_type& state, std::size_t size, std::size_t alignment)
    {
        return traits_detail::allocate_node(traits_detail::full_concept{}, state, size, alignment);
    }

    static void*
    allocate_array(
            allocator_type& state, std::size_t count, std::size_t size, std::size_t alignment)
    {
        return traits_detail::allocate_array(
                traits_detail::full_concept{}, state, count, size, alignment);
    }

    static void
    deallocate_node(
            allocator_type& state, void* node, std::size_t size, std::size_t alignment) noexcept
    {
        traits_detail::deallocate_node(traits_detail::full_concept{}, state, node, size, alignment);
    }

    static void
    deallocate_array(allocator_type& state, void* array, std::size_t count, std::size_t size,
            std::size_t alignment) noexcept
    {
        traits_detail::deallocate_array(
                traits_detail::full_concept{}, state, array, count, size, alignment);
    }

    static std::size_t
    max_node_size(const allocator_type& state)
    {
        return traits_detail::max_node_size(traits_detail::full_concept{}, state);
    }

    static std::size_t
    max_array_size(const allocator_type& state)
    {
        return traits_detail::max_array_size(traits_detail::full_concept{}, state);
    }

    static std::size_t
    max_alignment(const allocator_type& state)
    {
        return traits_detail::max_alignment(traits_detail::full_concept{}, state);
    }
};


// /// The default specialization of the allocator_traits for a
// /// \concept{concept_rawallocator,RawAllocator}. See the last link for the requirements on types
// /// that do not specialize this class and the interface documentation. Any specialization must
// /// provide the same interface. \ingroup memory core
// template <class Allocator>
// class allocator_traits
// {
// public:
//     using allocator_type = std::decay<Allocator>;
//     using is_stateful = std::true_type;

//     static void*
//     allocate_node(allocator_type& state, std::size_t size, std::size_t alignment)
//     {
//         return traits_detail::allocate_node(state, size, alignment);
//     }

//     static void*
//     allocate_array(
//             allocator_type& state, std::size_t count, std::size_t size, std::size_t alignment)
//     {
//         return traits_detail::allocate_array(state, count, size, alignment);
//     }

//     static void
//     deallocate_node(
//             allocator_type& state, void* node, std::size_t size, std::size_t alignment) noexcept
//     {
//         traits_detail::deallocate_node(state, node, size, alignment);
//     }

//     static void
//     deallocate_array(allocator_type& state, void* array, std::size_t count, std::size_t size,
//             std::size_t alignment) noexcept
//     {
//         traits_detail::deallocate_array(state, array, count, size, alignment);
//     }

//     static std::size_t
//     max_node_size(const allocator_type& state)
//     {
//         return traits_detail::max_node_size(state);
//     }

//     static std::size_t
//     max_array_size(const allocator_type& state)
//     {
//         return traits_detail::max_array_size(state);
//     }

//     static std::size_t
//     max_alignment(const allocator_type& state)
//     {
//         return traits_detail::max_alignment(state);
//     }
// };
