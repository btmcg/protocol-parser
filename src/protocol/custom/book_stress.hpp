#pragma once

#include "util/compiler.hpp"
#include <cstdint>


namespace book_stress {

    // Total of 5 msg types:
    //
    // note: types in parenthesis are mapped the same as the type listed above them
    //
    // type     itch_type   itch_name                   fields_contained
    //
    // A        A           add_order                   buy_sell, ts, oid,          qty, price
    //          (F)         add_order_with_mpid         buy_sell, ts, oid,          qty, price
    // C        C           order_executed_with_price             ts, oid,          qty, price
    //          (E)         order_executed                        ts, oid,          qty
    // D        D           order_delete                          ts, oid
    // U        U           order_replace                         ts, oid, new_oid, qty, price
    // X        X           order_cancel                          ts, oid,          qty


    /// any field that is undefined is set to all zeroes
    struct message
    {
        char type;
        std::uint8_t buy_sell; // 1=buy, 2=sell
        std::uint64_t ts;
        std::uint32_t oid;
        std::uint32_t new_oid;
        std::uint32_t qty;
        std::uint32_t price;
    } PACKED;
    static_assert(sizeof(message) == 26); // NOLINT

} // namespace book_stress
