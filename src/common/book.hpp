#pragma once

#include "allocator/memory_pool.hpp"
#include "allocator/std_allocator.hpp"
#include <cstdint>
#include <list>
#include <unordered_map>

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
};

/// Two-sided book
class tsbook
{
private:
    std::unordered_map<std::uint64_t, order> orders_;
    // std::list<price_level> bids_;
    // std::list<price_level> asks_;
    memory_pool bid_pool_;
    memory_pool ask_pool_;
    std::list<price_level, std_allocator<price_level, memory_pool>> bids_;
    std::list<price_level, std_allocator<price_level, memory_pool>> asks_;

public:
    tsbook() noexcept;
    void add_order(std::uint64_t order_num, Side, std::uint32_t price, std::uint32_t qty);
    void delete_order(std::uint64_t order_num) noexcept;
    void replace_order(std::uint64_t orig_order_num, std::uint64_t new_order_num,
            std::uint32_t price, std::uint32_t qty) noexcept;
    void cancel_order(std::uint64_t order_num, std::uint32_t cancelled_qty) noexcept;

    // accessors
public:
    std::unordered_map<std::uint64_t, order> const& order_list() const noexcept;
    decltype(bids_) const& bids() const noexcept;
    decltype(asks_) const& asks() const noexcept;
    price_level best_bid() const noexcept;
    price_level best_ask() const noexcept;
};
