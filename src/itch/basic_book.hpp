#pragma once

#include "core.hpp"
#include <cstddef> // std::size_t
#include <list>


// Book Interface:
// struct book
// {
//     book() noexcept;

//     void add_order(order&) noexcept;
//     void delete_order(order&) noexcept;
//     void replace_order(order& old_order, order& new_order) noexcept;
//     void cancel_order(order&, qty_t remove_qty) noexcept;

//     // accessors
//     decltype(bids_) const& bids() const noexcept;
//     decltype(asks_) const& asks() const noexcept;
//     price_level best_bid() const noexcept;
//     price_level best_ask() const noexcept;
// };

namespace itch {

    /// Two-sided book
    class basic_book
    {
    private:
        std::list<price_level> bids_;
        std::list<price_level> asks_;

    public:
        basic_book() noexcept;
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