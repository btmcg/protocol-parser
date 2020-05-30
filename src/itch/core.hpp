#pragma once

#include <cstdint>
#include <limits>


namespace itch {

    /// itch-specific price type
    using price_t = std::uint32_t;

    /// itch-specific quantity type
    using qty_t = std::uint32_t;


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

    struct order
    {
        Side side = Side::Bid;
        price_t price = 0;
        qty_t qty = 0;
        price_level* pl = nullptr;
        std::uint64_t ts = 0; ///< nsecs since epoch

        constexpr void
        clear() noexcept
        {
            side = Side::Bid;
            price = 0;
            qty = 0;
            pl = nullptr;
            ts = 0;
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
        return (p == InvalidHiPrice || p == InvalidLoPrice) ? 0.0
                                                            : static_cast<double>(p) / 10000.0;
    }

} // namespace itch
