#pragma once

#include <cstdint>
#include <limits>
#include <utility> // std::move


namespace itch {

    /// itch-specific price type
    using price_t = std::uint32_t;

    /// itch-specific quantity type
    using qty_t = std::uint32_t;


    constexpr std::uint8_t NameLen = 8;
    constexpr price_t InvalidHiPrice = std::numeric_limits<price_t>::min();
    constexpr price_t InvalidLoPrice = std::numeric_limits<price_t>::max();


    enum class Side : std::uint8_t
    {
        Bid = 0,
        Ask = 1
    };

    struct price_level
    {
        price_t price = 0;
        qty_t qty = 0;

        constexpr price_level(price_t p, qty_t q) noexcept
                : price(p)
                , qty(q)
        {
            // empty
        }
    };

    constexpr bool
    operator==(price_level const& lhs, price_level const& rhs) noexcept
    {
        return lhs.price == rhs.price && lhs.qty == rhs.qty;
    }

    struct order
    {
        price_t price = 0;
        qty_t qty = 0;
        price_level* pl = nullptr;
        std::uint64_t ts = 0; ///< nsecs since epoch
        Side side = Side::Bid;

        constexpr order(Side s, price_t p, qty_t q) noexcept
                : price(p)
                , qty(q)
                , side(s)
        {
            // empty
        }

        constexpr order() noexcept = default;
        constexpr ~order() noexcept = default;
        constexpr order(order const&) noexcept = default;
        order(order&&) noexcept = default;
        constexpr order& operator=(order const&) noexcept = default;
        order& operator=(order&& rhs) noexcept = default;

        void
        clear() noexcept
        {
            price = 0;
            qty = 0;
            pl = nullptr;
            ts = 0;
            side = Side::Bid;
        }
    };

    // clang-format off
    enum class MarketState : std::uint8_t
    {
        Open = 0,           // 0930 - 1600
        SystemUp,           // x - 0400
        AcceptingOrders,    // 0400 - 0930
        Closed,             // 1600 - 2000
        SystemDown,         // 2005 - x
        Unknown,
    };

    enum class InstrumentState : std::uint8_t
    {
        Trading = 0,
        Halted,             ///< halted market-wide
        OperationalHalted,  ///< only halted on nasdaq
        Paused,             ///< nasdaq-listed only
        QuotationOnly,
        Unknown,
    };
    // clang-format on

    /**********************************************************************/

    /// converts nasdaq integer price to human readable floating point
    constexpr inline double
    to_hr_price(price_t p) noexcept
    {
        // multiplying by the inverse avoids floating-point division and
        // therefore the fpu
        constexpr double inverse = 1 / 10000.0;
        return (p == InvalidHiPrice || p == InvalidLoPrice) ? 0.0 : p * inverse;
    }

} // namespace itch
