#include "hashed_book.hpp"
#include "util/assert.hpp"
#include <algorithm> // std::find_if
#include <cstdint>
#include <cstdio>  // std::fprintf
#include <cstdlib> // std::abort
#include <exception>


namespace { // unnamed

    // each std::list node has two pointers (prev, next) which we
    // should account for when creating memory pool
    constexpr std::uint32_t StdListNodeExtra = 16;

    // allocate this many price levels in the memory pool
    constexpr std::uint32_t NumPriceLevels = 50;

    // allocate this many buckets in the mapping
    constexpr std::uint32_t NumBuckets = 100;

} // namespace


namespace itch {

    hashed_book::hashed_book() noexcept
            : bid_pool_(sizeof(price_level) + StdListNodeExtra, NumPriceLevels)
            , ask_pool_(sizeof(price_level) + StdListNodeExtra, NumPriceLevels)
            , bids_(bid_pool_)
            , asks_(ask_pool_)
            , bid_map_()
            , ask_map_()
    {
        bid_map_.reserve(NumBuckets);
        ask_map_.reserve(NumBuckets);
    }

    void
    hashed_book::add_order(order& order) noexcept
    {
        auto* book = (order.side == Side::Bid) ? &bids_ : &asks_;
        auto* map = (order.side == Side::Bid) ? &bid_map_ : &ask_map_;

        // need to catch any exceptions from std::unordered_map::emplace
        try {
            if (book->empty()) {
                // initial price_level
                book->emplace_front(order);
                map->emplace(order.price, book->begin());
                order.pl = book->begin();
            } else {
                auto map_itr = map->find(order.price);

                // price not found in the map, therefore it doesn't exist in the
                // book. find out where it should go
                if (map_itr == map->end()) {
                    // find location in book
                    auto pl_itr
                            = std::find_if(book->begin(), book->end(), [&order](auto const& pl) {
                                  return (order.side == Side::Bid) ? pl.price() <= order.price
                                                                   : pl.price() >= order.price;
                              });

                    if (pl_itr == book->end()) {
                        // new price level (at bottome of book)
                        book->emplace_back(order);
                        order.pl = std::prev(book->end(), 1);
                        map->emplace(order.price, order.pl);
                    } else {
                        // new price level (middle of book)
                        order.pl = book->emplace(pl_itr, order);
                        map->emplace(order.price, order.pl);
                    }
                } else {
                    map_itr->second->add_order(order);
                    order.pl = map_itr->second;
                }
            }

            order.valid_pl = true;
        } catch (std::exception const& e) {
            std::fprintf(stderr, "[ERROR] exception in %s: %s\n", __builtin_FUNCTION(), e.what());
            std::abort();
        }
    }

    void
    hashed_book::delete_order(order& order) noexcept
    {
        DEBUG_ASSERT(order.valid_pl);
        DEBUG_ASSERT(order.qty <= order.pl->agg_qty());
        if (!order.valid_pl)
            return;

        DEBUG_ASSERT(order.qty <= order.pl->agg_qty());

        // decrease qty on price level, if it goes to zero, delete the level
        order.pl->delete_order(order);
        if (order.pl->size() == 0) {
            if (order.side == Side::Bid) {
                bid_map_.erase(order.price);
                bids_.erase(order.pl);
            } else {
                ask_map_.erase(order.price);
                asks_.erase(order.pl);
            }
        }

        order.clear();
    }

    void
    hashed_book::cancel_order(order& order, qty_t remove_qty) noexcept
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
    hashed_book::replace_order(order& old_order, order& new_order) noexcept
    {
        DEBUG_ASSERT(old_order.valid_pl);
        DEBUG_ASSERT(!new_order.valid_pl);

        delete_order(old_order);
        DEBUG_ASSERT(!old_order.valid_pl);

        add_order(new_order);
        DEBUG_ASSERT(new_order.valid_pl);
    }

    decltype(hashed_book::bids_) const&
    hashed_book::bids() const noexcept
    {
        return bids_;
    }

    decltype(hashed_book::asks_) const&
    hashed_book::asks() const noexcept
    {
        return asks_;
    }

    pq
    hashed_book::best_bid() const noexcept
    {
        if (bids_.empty())
            return {0, 0};

        return {bids_.front().price(), bids_.front().agg_qty()};
    }

    pq
    hashed_book::best_ask() const noexcept
    {
        if (asks_.empty())
            return {0, 0};

        return {asks_.front().price(), asks_.front().agg_qty()};
    }

    std::size_t
    hashed_book::max_bid_book_depth() const noexcept
    {
        return bid_pool_.max_used();
    }

    std::size_t
    hashed_book::max_ask_book_depth() const noexcept
    {
        return ask_pool_.max_used();
    }

    std::size_t
    hashed_book::max_bid_order_depth() const noexcept
    {
        return max_bid_order_depth_;
    }

    std::size_t
    hashed_book::max_ask_order_depth() const noexcept
    {
        return max_ask_order_depth_;
    }

} // namespace itch
