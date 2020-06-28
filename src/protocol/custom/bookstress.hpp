#pragma once

#include "util/compiler.hpp"
#include <cstdint>


namespace book_stress {

    // Total of 5 msg types:
    //
    //  A   add_order                   timestamp=14400667465235    order_reference_number=56305
    //  buy_sell=S  qty=300 price=3224000
    // (F)  add_order_with_mpid         timestamp=34233711766778    order_reference_number=11962301
    // buy_sell=B  qty=100 price=2964600
    //  C   order_executed_with_price   timestamp=34225117582112    order_reference_number=11680717
    //  qty=100 price=3208200
    // (E)  order_executed              timestamp=34225117844408    order_reference_number=11483893
    // qty=100
    //  D   order_delete                timestamp=34225117108121    order_reference_number=11679693
    //  U   order_replace               timestamp=34233726342065    order_reference_number=11962233
    //  qty=100 price=3446200   new_order_reference_number=11963441 X   order_cancel
    //  timestamp=34233960655710    order_reference_number=11966401             qty=100

    struct msg
    {
        char type;
        std::uint8_t buy_sell; // 1=buy, 2=sell
        std::uint64_t ts;
        std::uint32_t order_reference_number;
        std::uint32_t new_order_reference_number;
        std::uint32_t qty;
        std::uint32_t price;
    } PACKED;
    static_assert(sizeof(msg) == 26); // NOLINT

} // namespace book_stress
