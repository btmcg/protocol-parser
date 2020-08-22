#include "basic_book.hpp"
#include "util/assert.hpp"
#include <algorithm> // std::find_if
#include <cstdint>
#include <cstdio>  // std::fprintf
#include <cstdlib> // std::abort
#include <exception>


namespace itch {

    basic_book::basic_book() noexcept
            : bids_()
            , asks_()
    {
        // empty
    }

    void
    basic_book::add_order(order& order) noexcept
    {
        auto* book = (order.side == Side::Bid) ? &bids_ : &asks_;

        // emplace functions may throw, in which case we abort
        try {
            if (book->empty()) {
                order.pl = &book->emplace_front(order.price, order.qty);
            } else {
                // find location in book
                auto pl_itr = std::find_if(book->begin(), book->end(), [&order](auto const& pl) {
                    return (order.side == Side::Bid) ? pl.price() <= order.price
                                                     : pl.price() >= order.price;
                });

                if (pl_itr == book->end()) {
                    // establish price_level (at bottom of book)
                    order.pl = &book->emplace_back(order.price, order.qty);
                } else if (pl_itr->price() == order.price) {
                    // price exists, adjust qty
                    pl_itr->inc_qty(order.qty);
                    order.pl = &(*pl_itr);
                } else {
                    // establish new price level (in middle of book)
                    auto i_itr = book->emplace(pl_itr, order.price, order.qty);
                    order.pl = &(*i_itr);
                }
            }
        } catch (std::exception const& e) {
            std::fprintf(stderr, "[ERROR] exception in %s: %s\n", __builtin_FUNCTION(), e.what());
            std::abort();
        }
    }

    void
    basic_book::delete_order(order& order) noexcept
    {
        DEBUG_ASSERT(order.pl != nullptr);
        if (order.pl == nullptr)
            return;

        DEBUG_ASSERT(order.qty <= order.pl->agg_qty());

        // decrease qty on price level, if it goes to zero, delete the level
        if (order.qty < order.pl->agg_qty()) {
            order.pl->dec_qty(order.qty);
        } else {
            if (order.side == Side::Bid) {
                auto found_itr = std::find_if(bids_.begin(), bids_.end(),
                        [&order](auto& pl) { return order.price == pl.price(); });
                bids_.erase(found_itr);
            } else {
                auto found_itr = std::find_if(asks_.begin(), asks_.end(),
                        [&order](auto& pl) { return order.price == pl.price(); });
                asks_.erase(found_itr);
            }
        }

        order.clear();
    }

    void
    basic_book::cancel_order(order& order, qty_t remove_qty) noexcept
    {
        DEBUG_ASSERT(remove_qty <= order.qty);

        if (remove_qty < order.qty) {
            order.qty -= remove_qty;
            order.pl->dec_qty(remove_qty);
        } else {
            // this cancel will remove the order (may happen if caused
            // by an execution)
            delete_order(order);
        }
    }

    void
    basic_book::replace_order(order& old_order, order& new_order) noexcept
    {
        DEBUG_ASSERT(old_order.pl != nullptr);
        DEBUG_ASSERT(new_order.pl == nullptr);

        delete_order(old_order);
        DEBUG_ASSERT(old_order.pl == nullptr);

        add_order(new_order);
        DEBUG_ASSERT(new_order.pl != nullptr);
    }

    decltype(basic_book::bids_) const&
    basic_book::bids() const noexcept
    {
        return bids_;
    }

    decltype(basic_book::asks_) const&
    basic_book::asks() const noexcept
    {
        return asks_;
    }

    pq
    basic_book::best_bid() const noexcept
    {
        if (bids_.empty())
            return {0, 0};

        return {bids_.front().price(), bids_.front().agg_qty()};
    }

    pq
    basic_book::best_ask() const noexcept
    {
        if (asks_.empty())
            return {0, 0};

        return {asks_.front().price(), asks_.front().agg_qty()};
    }

} // namespace itch
