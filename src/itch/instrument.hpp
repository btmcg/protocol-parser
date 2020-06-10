#pragma once

#include "basic_book.hpp"
#include "mp_book.hpp"
#include "hashed_book.hpp"
#include "core.hpp"
#include <cstdint>
#include <string>


namespace itch {

    struct instrument
    {
        std::uint16_t locate = 0;
        char name[NameLen] = {0};
        mp_book book;

        price_t open = 0;
        price_t close = 0;
        price_t last = 0;
        price_t lo = InvalidHiPrice;
        price_t hi = InvalidLoPrice;
        std::uint32_t num_trades = 0;
        std::uint32_t trade_qty = 0;
        std::uint32_t num_orders = 0;
        InstrumentState instrument_state = InstrumentState::Unknown;

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
