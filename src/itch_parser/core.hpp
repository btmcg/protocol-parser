#pragma once

#include <cstdint>
#include <limits>


constexpr std::uint32_t InvalidPrice = 1'999'999'900;
constexpr std::uint32_t InvalidHiPrice = std::numeric_limits<std::uint32_t>::min();
constexpr std::uint32_t InvalidLoPrice = std::numeric_limits<std::uint32_t>::max();


enum Side : std::uint8_t
{
    Bid = 0,
    Ask = 1
};

struct price_level
{
    std::uint32_t price = 0;
    std::uint32_t qty = 0;

    price_level(std::uint32_t p, std::uint32_t q)
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
    Open = 0,               // 0930 - 1600
    SystemUp = 1,           // x - 0400
    AcceptingOrders = 2,    // 0400 - 0930
    Closed = 3,             // 1600 - 2000
    SystemDown = 4,         // 2005 - x
    Unknown = 5,
};
// clang-format on

/**********************************************************************/

constexpr inline double
to_hr_price(std::uint32_t p)
{
    return (p == InvalidHiPrice || p == InvalidLoPrice) ? 0.0 : static_cast<double>(p) / 10000.0;
}
