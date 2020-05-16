#include "book.hpp"
#include <fmt/format.h>
#include <algorithm>


tsbook::tsbook() noexcept
        : orders_()
        , bid_pool_(sizeof(price_level) + 16, 1000)
        , ask_pool_(sizeof(price_level) + 16, 1000)
        , bids_(bid_pool_)
        , asks_(ask_pool_)
        // , bids_()
        // , asks_()
{
    // empty
}

void
tsbook::add_order(std::uint64_t order_num, Side side, std::uint32_t price, std::uint32_t qty)
{
    order order{side, price, qty, nullptr};

    auto* book = (side == Bid) ? &bids_ : &asks_;

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
    auto itr = orders_.find(order_num);
    if (itr == orders_.end()) {
        fmt::print(stderr, "[ERROR] delete_order(): order number {} not found\n", order_num);
        return;
    }
    order& order = itr->second;

    // decrease qty on price level, if it goes to zero, delete the level
    if (order.pl->qty < order.qty) {
        order.pl->qty = 0;
        if (order.side == Side::Bid) {
            auto found = std::find_if(bids_.begin(), bids_.end(),
                    [&order](auto& pl) { return order.pl->price == pl.price; });
            auto d_itr = bids_.erase(found);
            if (d_itr == bids_.end()) {
                fmt::print(stderr, "[ERROR] delete_order(): can't find pl");
            }
        } else {
            auto found = std::find_if(asks_.begin(), asks_.end(),
                    [&order](auto& pl) { return order.pl->price == pl.price; });
            auto d_itr = asks_.erase(found);
            if (d_itr == asks_.end()) {
                fmt::print(stderr, "[ERROR] delete_order(): can't find pl");
            }
        }
    } else {
        order.pl->qty -= order.qty;
    }

    orders_.erase(itr);
}

void
tsbook::replace_order(std::uint64_t orig_order_num, std::uint64_t new_order_num,
        std::uint32_t price, std::uint32_t qty) noexcept
{
    auto itr = orders_.find(orig_order_num);
    if (itr == orders_.end()) {
        fmt::print(stderr, "[ERROR] replace_order(): order number {} not found\n", orig_order_num);
        return;
    }

    Side s = itr->second.side;

    delete_order(orig_order_num);
    add_order(new_order_num, s, price, qty);
}

void
tsbook::cancel_order(std::uint64_t order_num, std::uint32_t cancelled_qty) noexcept
{
    auto itr = orders_.find(order_num);
    if (itr == orders_.end()) {
        fmt::print(stderr, "[ERROR] cancel_order(): order number {} not found\n", order_num);
        return;
    }

    order& order = itr->second;

    if (cancelled_qty >= order.qty) {
        // this cancel will remove the order
        delete_order(order_num);
    } else {
        order.qty -= cancelled_qty;
        order.pl->qty -= cancelled_qty;
    }
}

std::unordered_map<std::uint64_t, order> const&
tsbook::order_list() const noexcept
{
    return orders_;
}

decltype(tsbook::bids_) const&
tsbook::bids() const noexcept
{
    return bids_;
}

decltype(tsbook::asks_) const&
tsbook::asks() const noexcept
{
    return asks_;
}

price_level
tsbook::best_bid() const noexcept
{
    if (bids_.empty())
        return {0, 0};

    return bids_.front();
}

price_level
tsbook::best_ask() const noexcept
{
    if (asks_.empty())
        return {0, 0};

    return asks_.front();
}

std::size_t
tsbook::bid_pool_capacity() const noexcept
{
    return bid_pool_.capacity();
}

std::size_t
tsbook::ask_pool_capacity() const noexcept
{
    return ask_pool_.capacity();
}
