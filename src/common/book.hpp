#pragma once

#include <cstdint>
#include <list>
#include <unordered_map>


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


class book
{
private:
    std::unordered_map<std::uint64_t, price_level> orders_; ///< order num to pl
    std::list<price_level> levels_;

public:
    enum Side
    {
        Buy = 0,
        Ask = 1
    };

public:
    book() noexcept;
    void add_order(std::uint64_t order_num, Side, std::uint32_t price, std::uint32_t qty);
};
