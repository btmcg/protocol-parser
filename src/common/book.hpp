#pragma once

#include <cstdint>
#include <unordered_map>

struct price_level
{
    std::uint32_t price = 0;
    std::uint32_t qty = 0;
};

class book
{
private:
    std::unordered_map<std::uint64_t, price_level> orders_;

public:
    void
    add(std::uint64_t order_num, std::uint32_t price, std::uint32_t qty)
    {}
    void
    cancel(std::uint64_t order_num, std::uint32_t qty)
    {}
    void
    cancel_full(std::uint64_t order_num)
    {}
    void
    replace(std::uint64_t order_num, std::uint64_t new_order_num, std::uint32_t price,
            std::uint32_t qty)
    {}
    void
    executed(std::uint64_t order_num, std::uint32_t qty)
    {}
};
