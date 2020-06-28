#pragma once

#include "util/compiler.hpp"
#include <cstdint>


namespace book_stress {

    // clang-format off
    // Total of 5 msg types:
    //
    // note: types in parenthesis are mapped the same as the type listed above them
    //
    // type     itch_type   itch_name                   fields_contained
    //
    // A        A           add_order                   side, oid,          qty, price
    //          (F)         add_order_with_mpid         side, oid,          qty, price
    // D        D           order_delete                      oid
    // E        E           order_executed                    oid,          qty
    //          (C)         order_executed_with_price         oid,          qty, price
    // U        U           order_replace                     oid, new_oid, qty, price
    // X        X           order_cancel                      oid,          qty
    // clang-format on


    struct header
    {
        char type;
        std::uint64_t ts;
    } PACKED;
    static_assert(sizeof(header) == 9); // NOLINT

    /// any field that is undefined is set to all zeroes
    struct message : public header
    {
        std::uint8_t side; // 0=buy, 1=sell, 2=n/a
        std::uint32_t oid;
        std::uint32_t new_oid;
        std::uint32_t qty;
        std::uint32_t price;
    } PACKED;
    static_assert(sizeof(message) == 26); // NOLINT

} // namespace book_stress
