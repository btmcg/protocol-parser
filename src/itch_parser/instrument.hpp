#pragma once

#include "book.hpp"
#include "core.hpp"
#include <cstdint>
#include <limits>
#include <string>


struct instrument
{
    std::uint16_t locate = 0;
    char name[8];
    tsbook book;

    std::uint32_t lo_price = InvalidHiPrice;
    std::uint32_t hi_price = InvalidLoPrice;
    std::uint32_t num_trades = 0;
    std::uint32_t trade_qty = 0;
    std::uint32_t num_orders = 0;
    InstrumentState instrument_state = InstrumentState::Unknown;

    instrument() noexcept;
    instrument(std::uint16_t locate, char const (&name)[8]) noexcept;
    void set_name(char const (&name)[8]) noexcept;

    // stats
    std::string stats_str() const noexcept;
    static std::string stats_csv_header() noexcept;
    std::string stats_csv() const noexcept;
    std::size_t allocator_stats() const noexcept;
};
