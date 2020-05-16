#pragma once

#include <cstdint>


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

    void
    clear() noexcept
    {
        side = Side::Bid;
        price = 0;
        qty = 0;
        pl = nullptr;
    }
};
