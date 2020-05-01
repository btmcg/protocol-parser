#include "book.hpp"
#include <fmt/format.h>
#include <algorithm>


book::book() noexcept
        : orders_()
        , levels_()
{
    // empty
}

void
book::add_order(std::uint64_t order_num, Side side, std::uint32_t price, std::uint32_t qty)
{
    auto itr = orders_.emplace(order_num, price_level{price, qty});
    if (!itr.second) {
        fmt::print(stderr, "add_order(): order_num={} already exists\n", order_num);
        return;
    }

    if (levels_.empty()) {
        levels_.emplace_front(price, qty);
        return;
    }

    auto o_itr = std::find_if(levels_.begin(), levels_.end(), [price, side](price_level const& pl) {
        return (side == Buy) ? pl.price <= price : pl.price >= price;
    });

    if (o_itr->price == price) {
        o_itr->qty += qty;
        return;
    }

    levels_.emplace(o_itr, price, qty);
}
