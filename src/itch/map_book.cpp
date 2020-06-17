#include "map_book.hpp"
#include <fmt/format.h>
#include <cstdint>


namespace { // unnamed

    // each std::map node needs extra memory for at least four pointers
    // which we should account for when creating memory pool
    constexpr std::uint32_t StdMapNodeExtra = sizeof(std::uintptr_t) * 4;

    // allocate this many price levels in the memory pool
    constexpr std::uint32_t NumPriceLevels = 5000;

} // namespace


namespace itch {

    map_book::map_book() noexcept
            // : bids_()
            // , asks_()
            : bid_pool_(
                    sizeof(std::pair<price_t const, price_level>) + StdMapNodeExtra, NumPriceLevels)
            , bids_(bid_pool_)
            , ask_pool_(sizeof(std::pair<price_t const, price_level>) + StdMapNodeExtra,
                      NumPriceLevels)
            , asks_(ask_pool_)
    {
        // empty
    }

    void
    map_book::add_order(order& order) noexcept
    {
        if (order.side == Side::Bid) {
            if (bids_.empty()) {
                auto [itr, success]
                        = bids_.emplace(order.price, price_level(order.price, order.qty));
                order.pl = &itr->second;
            } else {
                auto o_itr = bids_.find(order.price);
                if (o_itr == bids_.end()) {
                    auto [itr, success]
                            = bids_.emplace(order.price, price_level(order.price, order.qty));
                    order.pl = &itr->second;
                } else {
                    o_itr->second.qty += order.qty;
                    order.pl = &(o_itr->second);
                }
            }
        } else if (order.side == Side::Ask) {
            if (asks_.empty()) {
                auto [itr, success]
                        = asks_.emplace(order.price, price_level(order.price, order.qty));
                order.pl = &itr->second;
            } else {
                auto o_itr = asks_.find(order.price);
                if (o_itr == asks_.end()) {
                    auto [itr, success]
                            = asks_.emplace(order.price, price_level(order.price, order.qty));
                    order.pl = &itr->second;
                } else {
                    o_itr->second.qty += order.qty;
                    order.pl = &(o_itr->second);
                }
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

    void
    map_book::replace_order(order& old_order, order& new_order) noexcept
    {
        delete_order(old_order);
        add_order(new_order);
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
        return bids_.empty() ? price_level(0, 0) : bids_.begin()->second;
    }

    price_level
    map_book::best_ask() const noexcept
    {
        return asks_.empty() ? price_level(0, 0) : asks_.begin()->second;
    }

    std::size_t
    map_book::max_bid_pool_used() const noexcept
    {
        return bid_pool_.max_used();
    }

    std::size_t
    map_book::max_ask_pool_used() const noexcept
    {
        return ask_pool_.max_used();
    }

} // namespace itch
