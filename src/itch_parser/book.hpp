#pragma once

#include "core.hpp"
#include "allocator/memory_pool.hpp"
#include "allocator/std_allocator.hpp"
#include <cstdint>
#include <list>


/// Two-sided book
class tsbook
{
private:
    memory_pool bid_pool_;
    memory_pool ask_pool_;
    std::list<price_level, std_allocator<price_level, memory_pool>> bids_;
    std::list<price_level, std_allocator<price_level, memory_pool>> asks_;

public:
    tsbook() noexcept;
    void add_order(order&) noexcept;
    void delete_order(order&) noexcept;
    void replace_order(order& old_order, order& new_order) noexcept;
    void cancel_order(order&, std::uint32_t remove_qty) noexcept;

    // accessors
public:
    decltype(bids_) const& bids() const noexcept;
    decltype(asks_) const& asks() const noexcept;
    price_level best_bid() const noexcept;
    price_level best_ask() const noexcept;
    std::size_t bid_pool_capacity() const noexcept;
    std::size_t ask_pool_capacity() const noexcept;
};
