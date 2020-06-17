#pragma once

#include <cstdint>
#include <limits>
#include <list>
#include <utility> // std::move


namespace itch {

    /// itch-specific price type
    using price_t = std::uint32_t;

    /// itch-specific quantity type
    using qty_t = std::uint32_t;

    /// itch-specific order reference number
    using oid_t = std::uint64_t;

    /// maximum size of stock name
    constexpr std::uint8_t NameLen = 8;

    /// initial value for daily high price
    constexpr price_t InvalidHiPrice = std::numeric_limits<price_t>::min();

    /// initial value for daily low price
    constexpr price_t InvalidLoPrice = std::numeric_limits<price_t>::max();


    enum class Side : std::uint8_t
    {
        Bid = 0,
        Ask = 1
    };

    /// trivial price/quantity container
    struct pq
    {
        price_t price = 0;
        qty_t qty = 0;

        constexpr bool operator==(pq const&) const noexcept = default;
    };

    class price_level; // forward dec

    struct order
    {
        price_t price = 0;
        qty_t qty = 0;
        std::list<price_level>::iterator pl;
        std::uint64_t ts = 0; ///< nsecs since epoch
        Side side = Side::Bid;
        bool valid_pl = false;

        order(Side s, price_t p, qty_t q) noexcept
                : price(p)
                , qty(q)
                , pl()
                , side(s)
        {
            // empty
        }

        order() noexcept = default;
        ~order() noexcept = default;
        order(order const&) noexcept = default;
        order(order&&) noexcept = default;
        order& operator=(order const&) noexcept = default;
        order& operator=(order&& rhs) noexcept = default;
        bool operator==(order const&) const noexcept = default;

        void
        clear() noexcept
        {
            price = 0;
            qty = 0;
            ts = 0;
            side = Side::Bid;
            valid_pl = false;
        }
    };

    enum class MarketState : std::uint8_t
    {
        // clang-format off
        Open = 0,           // 0930 - 1600
        SystemUp,           // x - 0400
        AcceptingOrders,    // 0400 - 0930
        Closed,             // 1600 - 2000
        SystemDown,         // 2005 - x
        Unknown,
        // clang-format on
    };

    enum class InstrumentState : std::uint8_t
    {
        // clang-format off
        Trading = 0,
        Halted,             ///< halted market-wide
        OperationalHalted,  ///< only halted on nasdaq
        Paused,             ///< nasdaq-listed only
        QuotationOnly,
        Unknown,
        // clang-format on
    };

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
