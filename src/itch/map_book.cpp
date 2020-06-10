#include "map_book.hpp"
#include <fmt/format.h>
#include <algorithm>
#include <cstdint>

namespace itch {

    map_book::map_book() noexcept
            : bids_()
            , asks_()
    {
        // empty
    }

    void
    map_book::add_order(order& order) noexcept
    {
        auto book = (order.side == Side::Bid) ? &bids_ : &asks_;

        if (book->empty()) {
            auto [itr, success] = book->insert(
                    std::make_pair(order.price, price_level(order.price, order.qty)));
            order.pl = &itr->second;
        } else {
            auto o_itr = book->find(order.price);
            if (o_itr == book->end()) {
                auto [itr, success] = book->insert(
                        std::make_pair(order.price, price_level(order.price, order.qty)));
                order.pl = &itr->second;
            } else {
                o_itr->second.qty += order.qty;
                order.pl = &(o_itr->second);
            }
        }
    }

    void
    map_book::delete_order(order& order) noexcept
    {
        if (order.pl == nullptr)
            return;

        // decrease qty on price level, if it goes to zero, delete the level
        if (order.pl->qty <= order.qty) {
            order.pl->qty = 0;
            if (order.side == Side::Bid) {
                auto found_itr = bids_.find(order.price);
                bids_.erase(found_itr);
            } else {
                auto found_itr = asks_.find(order.price);
                asks_.erase(found_itr);
            }
        } else {
            order.pl->qty -= order.qty;
        }

        order.clear();
    }

    void
    map_book::replace_order(order& old_order, order& new_order) noexcept
    {
        delete_order(old_order);
        add_order(new_order);
    }

    void
    map_book::cancel_order(order& order, qty_t remove_qty) noexcept
    {
        if (remove_qty >= order.qty) {
            // this cancel will remove the order
            delete_order(order);
        } else {
            order.qty -= remove_qty;
            order.pl->qty -= remove_qty;
        }
    }

    decltype(map_book::bids_) const&
    map_book::bids() const noexcept
    {
        return bids_;
    }

    decltype(map_book::asks_) const&
    map_book::asks() const noexcept
    {
        return asks_;
    }

    price_level
    map_book::best_bid() const noexcept
    {
        if (bids_.empty())
            return {0, 0};

        auto itr = std::max_element(bids_.cbegin(), bids_.cend(),
                [](auto const& l, auto const& r) { return l.first < r.first; });

        return itr->second;
    }

    price_level
    map_book::best_ask() const noexcept
    {
        if (asks_.empty())
            return {0, 0};

        auto itr = std::min_element(asks_.cbegin(), asks_.cend(),
                [](auto const& l, auto const& r) { return l.first < r.first; });

        return itr->second;
    }

} // namespace itch
