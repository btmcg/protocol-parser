#pragma once

#include <cstdint>


constexpr std::uint32_t InvalidPrice = 1'999'999'900;
constexpr std::uint32_t InvalidHiPrice = 1'999'990'000;
constexpr std::uint32_t InvalidLoPrice = 1;


enum Side
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

/**********************************************************************/

constexpr inline double
to_hr_price(std::uint32_t p)
{
    return (p == InvalidHiPrice || p == InvalidLoPrice) ? 0.0 : static_cast<double>(p) / 10000.0;
}
