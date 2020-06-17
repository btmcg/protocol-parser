#include "hashed_book.hpp"
#include "util/assert.hpp"
#include <algorithm> // std::find_if
#include <cstdint>
#include <cstdio> // std::fprintf
#include <cstdlib> // std::abort
#include <exception>


namespace { // unnamed

    // each std::list node has two pointers (prev, next) which we
    // should account for when creating memory pool
    constexpr std::uint32_t StdListNodeExtra = 16;

    // allocate this many price levels in the memory pool
    constexpr std::uint32_t NumPriceLevels = 5000;

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
    hashed_book::add_order(order& o) noexcept
    {
        auto* book = (o.side == Side::Bid) ? &bids_ : &asks_;
        auto* map = (o.side == Side::Bid) ? &bid_map_ : &ask_map_;

        // need to catch any exceptions from std::unordered_map::emplace
        try {
            if (book->empty()) {
                auto new_itr = book->emplace(book->begin(), o.price, o.qty);
                map->emplace(o.price, new_itr);
                o.pl = &(*new_itr);
            } else {
                auto map_itr = map->find(o.price);

                // price not found in the map, therefore it doesn't exist in the
                // book. find out where it should go
                if (map_itr == map->end()) {
                    // find location in book
                    auto loc = std::find_if(book->begin(), book->end(), [&o](auto const& pl) {
                        return (o.side == Side::Bid) ? pl.price() <= o.price
                                                     : pl.price() >= o.price;
                    });

                    // new price level
                    auto new_itr = book->emplace(loc, o.price, o.qty);
                    map->emplace(o.price, new_itr);
                    o.pl = &(*new_itr);
                } else {
                    map_itr->second->inc_qty(o.qty);
                    o.pl = &*(map_itr->second);
                }
            }
        } catch (std::exception const& e) {
            std::fprintf(stderr, "[ERROR] exception in %s: %s\n", __builtin_FUNCTION(), e.what());
            std::abort();
        }
    }

    void
    hashed_book::delete_order(order& order) noexcept
    {
        DEBUG_ASSERT(order.pl != nullptr);
        if (order.pl == nullptr)
            return;

        DEBUG_ASSERT(order.qty <= order.pl->agg_qty());

        // decrease qty on price level, if it goes to zero, delete the level
        if (order.qty < order.pl->agg_qty()) {
            order.pl->dec_qty(order.qty);
        } else {
            auto* book = (order.side == Side::Bid) ? &bids_ : &asks_;
            auto* map = (order.side == Side::Bid) ? &bid_map_ : &ask_map_;

            auto map_itr = map->find(order.pl->price());
            book->erase(map_itr->second);
            map->erase(map_itr);
        }

        order.clear();
    }

    void
    hashed_book::cancel_order(order& order, qty_t remove_qty) noexcept
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
    hashed_book::replace_order(order& old_order, order& new_order) noexcept
    {
        DEBUG_ASSERT(old_order.pl != nullptr);
        DEBUG_ASSERT(new_order.pl == nullptr);

        delete_order(old_order);
        DEBUG_ASSERT(old_order.pl == nullptr);

        add_order(new_order);
        DEBUG_ASSERT(new_order.pl != nullptr);
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
    hashed_book::max_bid_pool_used() const noexcept
    {
        return bid_pool_.max_used();
    }

    std::size_t
    hashed_book::max_ask_pool_used() const noexcept
    {
        return ask_pool_.max_used();
    }

} // namespace itch
