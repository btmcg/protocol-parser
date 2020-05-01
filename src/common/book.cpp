#include "book.hpp"
#include <fmt/format.h>
#include <algorithm>


tsbook::tsbook() noexcept
        : orders_()
        , bids_()
        , asks_()
{
    // empty
}

void
tsbook::add_order(std::uint64_t order_num, Side side, std::uint32_t price, std::uint32_t qty)
{
    order order{side, price, qty, nullptr};

    std::list<price_level>* book = (side == Bid) ? &bids_ : &asks_;

    if (book->empty()) {
        order.pl = &book->emplace_front(price, qty);
    } else {
        // find location in book
        auto o_itr = std::find_if(book->begin(), book->end(), [price, side](price_level const& pl) {
            return (side == Bid) ? pl.price <= price : pl.price >= price;
        });

        if (o_itr->price == price) {
            // price exists, adjust qty
            o_itr->qty += qty;
            order.pl = &(*o_itr);
        } else {
            // new price level
            auto i_itr = book->emplace(o_itr, price, qty);
            order.pl = &(*i_itr);
        }
    }

    // record the order
    if (orders_.emplace(order_num, order).second == false) {
        fmt::print(stderr, "[ERROR] add_order(): order_num={} already exists\n", order_num);
    }
}


void
tsbook::delete_order(std::uint64_t order_num) noexcept
{
    auto o_itr = orders_.find(order_num);
    if (o_itr == orders_.end()) {
        fmt::print(stderr, "[ERROR] delete_order(): order_num={} doesn't exist\n", order_num);
        return;
    }

    order* order = (&o_itr->second);
    // decrease qty on price level, if it goes to zero, delete the level
    if (order->pl->qty < order->qty) {
        order->pl->qty = 0;
        if (order->side == Side::Bid) {
            auto found = std::find_if(bids_.begin(), bids_.end(),
                    [order](auto& pl) { return order->pl->price == pl.price; });
            auto d_itr = bids_.erase(found);
            if (d_itr == bids_.end()) {
                fmt::print(stderr, "[ERROR] delete_order(): can't find pl");
            }
        } else {
            auto found = std::find_if(asks_.begin(), asks_.end(),
                    [order](auto& pl) { return order->pl->price == pl.price; });
            auto d_itr = asks_.erase(found);
            if (d_itr == asks_.end()) {
                fmt::print(stderr, "[ERROR] delete_order(): can't find pl");
            }
        }
    } else {
        order->pl->qty -= order->qty;
    }

    orders_.erase(o_itr);
}


std::unordered_map<std::uint64_t, order> const&
tsbook::order_list() const noexcept
{
    return orders_;
}

std::list<price_level> const&
tsbook::bids() const noexcept
{
    return bids_;
}

std::list<price_level> const&
tsbook::asks() const noexcept
{
    return asks_;
}

