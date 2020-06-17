#include "mp_book.hpp"
#include "util/assert.hpp"
#include <algorithm> // std::find_if
#include <cstdint>
#include <cstdio>  // std::fprintf
#include <cstdlib> // std::abort
#include <exception>


namespace { // unnamed

    // each std::list node has two pointers (prev, next) which we
    // should account for when creating memory pool
    constexpr std::uint32_t StdListNodeExtra = sizeof(std::uintptr_t) + sizeof(std::uintptr_t);

    // allocate this many price levels in the memory pool
    constexpr std::uint32_t NumPriceLevels = 50;

} // namespace


namespace itch {

    mp_book::mp_book() noexcept
            : bid_pool_(sizeof(price_level) + StdListNodeExtra, NumPriceLevels)
            , ask_pool_(sizeof(price_level) + StdListNodeExtra, NumPriceLevels)
            , bids_(bid_pool_)
            , asks_(ask_pool_)
    {
        // empty
    }

    void
    mp_book::add_order(order& order) noexcept
    {
        auto* book = (order.side == Side::Bid) ? &bids_ : &asks_;

        // emplace functions may throw, in which case we abort
        try {
            if (book->empty()) {
                // initial price_level
                book->emplace_front(order);
                order.pl = book->begin();
            } else {
                // find location in book
                auto pl_itr = std::find_if(book->begin(), book->end(), [&order](auto const& pl) {
                    return (order.side == Side::Bid) ? pl.price() <= order.price
                                                     : pl.price() >= order.price;
                });

                if (pl_itr == book->end()) {
                    // new price_level (at bottom of book)
                    book->emplace_back(order);
                    order.pl = std::prev(book->end(), 1);
                } else if (pl_itr->price() == order.price) {
                    // price exists, adjust qty
                    pl_itr->add_order(order);
                    order.pl = pl_itr;
                } else {
                    // new price level (middle of book)
                    order.pl = book->emplace(pl_itr, order);
                }
            }

            order.valid_pl = true;
        } catch (std::exception const& e) {
            std::fprintf(stderr, "[ERROR] exception in %s: %s\n", __builtin_FUNCTION(), e.what());
            std::abort();
        }
    }

    void
    mp_book::delete_order(order& order) noexcept
    {
        DEBUG_ASSERT(order.valid_pl);
        DEBUG_ASSERT(order.qty <= order.pl->agg_qty());
        if (!order.valid_pl)
            return;

        // if order is the last on a price_level, delete the level
        order.pl->delete_order(order);
        if (order.pl->size() == 0) {
            if (order.side == Side::Bid) {
                max_bid_order_depth_ = std::max(max_bid_order_depth_, order.pl->max_orders());
                bids_.erase(order.pl);
            } else {
                max_ask_order_depth_ = std::max(max_ask_order_depth_, order.pl->max_orders());
                asks_.erase(order.pl);
            }
        }

        order.clear();
    }

    void
    mp_book::cancel_order(order& order, qty_t remove_qty) noexcept
    {
        DEBUG_ASSERT(remove_qty <= order.qty);

        if (remove_qty < order.qty) {
            order.qty -= remove_qty;
            order.pl->cancel_order(order, remove_qty);
        } else {
            // this cancel will remove the order (may happen if caused
            // by an execution)
            delete_order(order);
        }
    }

    void
    mp_book::replace_order(order& old_order, order& new_order) noexcept
    {
        DEBUG_ASSERT(old_order.valid_pl);
        DEBUG_ASSERT(!new_order.valid_pl);

        delete_order(old_order);
        DEBUG_ASSERT(!old_order.valid_pl);

        add_order(new_order);
        DEBUG_ASSERT(new_order.valid_pl);
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

    pq
    mp_book::best_bid() const noexcept
    {
        if (bids_.empty())
            return {0, 0};

        return {bids_.front().price(), bids_.front().agg_qty()};
    }

    pq
    mp_book::best_ask() const noexcept
    {
        if (asks_.empty())
            return {0, 0};

        return {asks_.front().price(), asks_.front().agg_qty()};
    }

    std::size_t
    mp_book::max_bid_book_depth() const noexcept
    {
        return bid_pool_.max_used();
    }

    std::size_t
    mp_book::max_ask_book_depth() const noexcept
    {
        return ask_pool_.max_used();
    }

    std::size_t
    mp_book::max_bid_order_depth() const noexcept
    {
        return max_bid_order_depth_;
    }

    std::size_t
    mp_book::max_ask_order_depth() const noexcept
    {
        return max_ask_order_depth_;
    }

} // namespace itch
