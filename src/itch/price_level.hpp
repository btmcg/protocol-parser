#pragma once

#include "core.hpp"


namespace itch {

    class price_level
    {
    private:
        price_t price_ = 0;
        qty_t agg_qty_ = 0; // sum of all order qtys on the level

    public:
        constexpr price_level(price_t p, qty_t q) noexcept
                : price_(p)
                , agg_qty_(q)
        {
            // empty
        }

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

        void
        inc_qty(qty_t q) noexcept
        {
            agg_qty_ += q;
        }

        void
        dec_qty(qty_t q) noexcept
        {
            agg_qty_ -= q;
        }

        constexpr bool
        operator==(price_level const& rhs) const noexcept
        {
            return price_ == rhs.price_ && agg_qty_ == rhs.agg_qty_;
        }
    };

} // namespace itch
