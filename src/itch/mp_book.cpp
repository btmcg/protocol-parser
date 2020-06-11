#include "mp_book.hpp"
#include <algorithm>
#include <cstdint>


namespace { // unnamed

    // each std::list node has two pointers (prev, next) which we
    // should account for when creating memory pool
    constexpr std::uint32_t StdListNodeExtra = sizeof(std::uintptr_t) + sizeof(std::uintptr_t);

    // allocate this many price levels in the memory pool
    constexpr std::uint32_t NumPriceLevels = 5000;

} // namespace


namespace itch {

    mp_book::mp_book() noexcept
            : bid_pool_(sizeof(price_level) + StdListNodeExtra, NumPriceLevels)
            , bids_(bid_pool_)
            , ask_pool_(sizeof(price_level) + StdListNodeExtra, NumPriceLevels)
            , asks_(ask_pool_)
    {
        // empty
    }

    void
    mp_book::add_order(order& order) noexcept
    {
        auto* book = (order.side == Side::Bid) ? &bids_ : &asks_;

        if (book->empty()) {
            order.pl = &book->emplace_front(order.price, order.qty);
        } else {
            // find location in book
            auto o_itr = std::find_if(book->begin(), book->end(), [&order](price_level const& pl) {
                return (order.side == Side::Bid) ? pl.price <= order.price
                                                 : pl.price >= order.price;
            });

            if (o_itr == book->end()) {
                order.pl = &book->emplace_back(order.price, order.qty);
            } else if (o_itr->price == order.price) {
                // price exists, adjust qty
                o_itr->qty += order.qty;
                order.pl = &(*o_itr);
            } else {
                // new price level
                auto i_itr = book->emplace(o_itr, order.price, order.qty);
                order.pl = &(*i_itr);
            }
        }
    }

    void
    mp_book::delete_order(order& order) noexcept
    {
        if (order.pl == nullptr)
            return;

        // decrease qty on price level, if it goes to zero, delete the level
        if (order.pl->qty <= order.qty) {
            order.pl->qty = 0;
            if (order.side == Side::Bid) {
                auto found = std::find_if(bids_.begin(), bids_.end(),
                        [&order](auto& pl) { return order.pl->price == pl.price; });
                bids_.erase(found);
            } else {
                auto found = std::find_if(asks_.begin(), asks_.end(),
                        [&order](auto& pl) { return order.pl->price == pl.price; });
                asks_.erase(found);
            }
        } else {
            order.pl->qty -= order.qty;
        }

        order.clear();
    }

    void
    mp_book::replace_order(order& old_order, order& new_order) noexcept
    {
        delete_order(old_order);
        add_order(new_order);
    }

    void
    mp_book::cancel_order(order& order, qty_t remove_qty) noexcept
    {
        if (remove_qty >= order.qty) {
            // this cancel will remove the order
            delete_order(order);
        } else {
            order.qty -= remove_qty;
            order.pl->qty -= remove_qty;
        }
    }

    decltype(mp_book::bids_) const&
    mp_book::bids() const noexcept
    {
        return bids_;
    }

    decltype(mp_book::asks_) const&
    mp_book::asks() const noexcept
    {
        return asks_;
    }

    price_level
    mp_book::best_bid() const noexcept
    {
        if (bids_.empty())
            return {0, 0};

        return bids_.front();
    }

    price_level
    mp_book::best_ask() const noexcept
    {
        if (asks_.empty())
            return {0, 0};

        return asks_.front();
    }

    std::size_t
    mp_book::max_bid_pool_used() const noexcept
    {
        return bid_pool_.max_used();
    }

    std::size_t
    mp_book::max_ask_pool_used() const noexcept
    {
        return ask_pool_.max_used();
    }

} // namespace itch
