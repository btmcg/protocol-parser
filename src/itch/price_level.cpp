#include "price_level.hpp"
#include "util/assert.hpp"
#include <algorithm> // std::find
#include <cstdint>
#include <cstdio> // std::fprintf
#include <cstdlib> // std::abort
#include <exception>


namespace itch {

    namespace { // unnamed

        // each std::list node has two pointers (prev, next) which we
        // should account for when creating memory pool
        constexpr std::uint32_t StdListNodeExtra = sizeof(std::uintptr_t) + sizeof(std::uintptr_t);

        // allocate this many orders in the memory pool
        constexpr std::uint32_t NumOrders = 100;

    } // namespace


    price_level::price_level(order const& o) noexcept
            : price_(o.price)
            , order_pool_(sizeof(order*) + StdListNodeExtra, NumOrders)
            , orders_(order_pool_)
    {
        add_order(o);
    }

    void
    price_level::add_order(order const& order) noexcept
    {
        DEBUG_ASSERT(order.price == price_);

        try {
            orders_.emplace_back(&order);
        } catch (std::exception const& e) {
            std::fprintf(stderr, "[%s] exception caught, aborting: %s\n", __builtin_FUNCTION(),
                    e.what());
            std::abort();
        }

        agg_qty_ += order.qty;
        max_orders_ = std::max(max_orders_, size());
    }

    void
    price_level::delete_order(order const& order) noexcept
    {
        DEBUG_ASSERT(order.price == price_);
        DEBUG_ASSERT(order.qty <= agg_qty_);

        auto found_itr = std::find(orders_.begin(), orders_.end(), &order);

        DEBUG_ASSERT(found_itr != orders_.end());
        if (found_itr == orders_.end())
            return;

        orders_.erase(found_itr);
        agg_qty_ -= order.qty;
    }

    void
    price_level::cancel_order(order const&, qty_t remove_qty) noexcept
    {
        DEBUG_ASSERT(remove_qty < agg_qty_);
        agg_qty_ -= remove_qty;
    }

} // namespace itch
