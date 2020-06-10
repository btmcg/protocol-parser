#pragma once

#include "core.hpp"
#include "allocator/memory_pool.hpp"
#include "allocator/mp_allocator.hpp"
#include <cstddef> // std::size_t
#include <list>


namespace itch {

    /// Two-sided book with memory-pooled bids and asks
    class mp_book
    {
    private:
        memory_pool bid_pool_;
        std::list<price_level, mp_allocator<price_level>> bids_;
        memory_pool ask_pool_;
        std::list<price_level, mp_allocator<price_level>> asks_;

    public:
        mp_book() noexcept;
        void add_order(order&) noexcept;
        void delete_order(order&) noexcept;
        void replace_order(order& old_order, order& new_order) noexcept;
        void cancel_order(order&, qty_t remove_qty) noexcept;

        // accessors
    public:
        decltype(bids_) const& bids() const noexcept;
        decltype(asks_) const& asks() const noexcept;
        price_level best_bid() const noexcept;
        price_level best_ask() const noexcept;
        std::size_t max_bid_pool_used() const noexcept;
        std::size_t max_ask_pool_used() const noexcept;
    };

} // namespace itch
