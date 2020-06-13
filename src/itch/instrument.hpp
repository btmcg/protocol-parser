#pragma once

#include "basic_book.hpp"
#include "core.hpp"
#include "hashed_book.hpp"
#include "map_book.hpp"
#include "mp_book.hpp"
#include <cstdint>
#include <string>


namespace itch {

    struct instrument
    {
        mp_book book;
        // pahole: --- cacheline 2 boundary (128 bytes) was 48 bytes ago ---
        std::uint32_t num_orders = 0;
        char name[NameLen] = {0};
        std::uint16_t locate = 0;
        InstrumentState instrument_state = InstrumentState::Unknown;

        // pahole: XXX 1 byte hole, try to pack
        // pahole: --- cacheline 3 boundary (192 bytes) ---
        std::uint32_t trade_qty = 0;
        std::uint32_t num_trades = 0;
        price_t lo = InvalidHiPrice;
        price_t hi = InvalidLoPrice;
        price_t last = 0;
        price_t open = 0;
        price_t close = 0;

        instrument() noexcept;
        instrument(std::uint16_t locate, char const (&name)[NameLen]) noexcept;
        void set_name(char const (&name)[NameLen]) noexcept;

        // stats
        std::string stats_str() const;
        static std::string stats_csv_header() noexcept;
        std::string stats_csv() const;
        std::pair<std::size_t, std::size_t> allocator_stats() const noexcept;
    };

} // namespace itch
