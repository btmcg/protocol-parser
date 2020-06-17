#pragma once

#include "core.hpp"
#include "allocator/memory_pool.hpp"
#include "allocator/mp_allocator.hpp"
#include <list>


namespace itch {

    class price_level
    {
    private:
        price_t price_ = 0;
        qty_t agg_qty_ = 0; ///< sum of all order qtys on the level
        std::size_t max_orders_ = 0; ///< stats only
        memory_pool order_pool_;
        std::list<order const*, mp_allocator<order const*>> orders_;

    public:
        explicit price_level(order const&) noexcept;

        void add_order(order const&) noexcept;
        void delete_order(order const&) noexcept;
        void cancel_order(order const&, qty_t remove_qty) noexcept;

        constexpr price_t
        price() const noexcept
        {
            return price_;
        }

        constexpr qty_t
        agg_qty() const noexcept
        {
            return agg_qty_;
        }

        /*constexpr*/ std::size_t
        size() const noexcept
        {
            return orders_.size();
        }

        constexpr std::size_t
        max_orders() const noexcept
        {
            return max_orders_;
        }

        auto const&
        orders() const noexcept
        {
            return orders_;
        }

        bool
        operator==(price_level const& rhs) const noexcept
        {
            return price_ == rhs.price_ && agg_qty_ == rhs.agg_qty_ && orders_ == rhs.orders_;
        }
    };

} // namespace itch
