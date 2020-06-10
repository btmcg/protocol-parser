#pragma once

#include "core.hpp"
#include <cstddef> // std::size_t
#include <map>


namespace itch {

    /// Two-sided book
    class map_book
    {
    private:
        std::map<price_t, price_level> bids_;
        std::map<price_t, price_level> asks_;

    public:
        map_book() noexcept;
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
        std::size_t
        max_bid_pool_used() const noexcept
        {
            return 0;
        }
        std::size_t
        max_ask_pool_used() const noexcept
        {
            return 0;
        }
    };

} // namespace itch
