#pragma once

#include <cstdint>
#include <limits>


namespace itch {

    constexpr std::uint32_t InvalidHiPrice = std::numeric_limits<std::uint32_t>::min();
    constexpr std::uint32_t InvalidLoPrice = std::numeric_limits<std::uint32_t>::max();


    enum class Side : std::uint8_t
    {
        Bid = 0,
        Ask = 1
    };

    struct price_level
    {
        std::uint32_t price = 0;
        std::uint32_t qty = 0;

        price_level(std::uint32_t p, std::uint32_t q) noexcept
                : price(p)
                , qty(q)
        {
            // empty
        }
    };

    struct order
    {
        Side side = Side::Bid;
        std::uint32_t price = 0;
        std::uint32_t qty = 0;
        price_level* pl = nullptr;
        std::uint64_t ts = 0;

        void
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
    to_hr_price(std::uint32_t p) noexcept
    {
        return (p == InvalidHiPrice || p == InvalidLoPrice) ? 0.0
                                                            : static_cast<double>(p) / 10000.0;
    }

} // namespace itch
