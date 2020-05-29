#pragma once

#include "core.hpp"
#include "allocator/memory_pool.hpp"
#include "allocator/mp_allocator.hpp"
#include <cstdint>
#include <list>
#include <unordered_map>


namespace itch {
    /// Book with std::list for ordered price levels and a
    /// std::unordered_map keyed by price for fast look-up.
    class mapped_book
    {
    private:
        memory_pool bid_pool_;
        memory_pool ask_pool_;
        std::list<price_level, mp_allocator<price_level>> bids_;
        std::list<price_level, mp_allocator<price_level>> asks_;
        std::unordered_map<std::uint32_t, decltype(bids_)::iterator> bid_map_;
        std::unordered_map<std::uint32_t, decltype(asks_)::iterator> ask_map_;

    public:
        mapped_book() noexcept;
        void add_order(order&) noexcept;
        void delete_order(order&) noexcept;
        void replace_order(order& old_order, order& new_order) noexcept;
        void cancel_order(order&, std::uint32_t remove_qty) noexcept;

        // accessors
    public:
        decltype(bids_) const& bids() const noexcept;
        decltype(asks_) const& asks() const noexcept;
        price_level best_bid() const noexcept;
        price_level best_ask() const noexcept;
    };

} // namespace itch
