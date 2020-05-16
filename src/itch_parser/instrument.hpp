#pragma once

#include "book.hpp"
#include <cstdint>
#include <limits>
#include <string>

constexpr inline double to_hr_price(std::uint32_t p);

struct instrument
{
    // static constexpr std::uint32_t InvalidPrice = 0x77359400;
    // static constexpr std::uint32_t InvalidPrice = 1'999'990'000;
    static constexpr std::uint32_t InvalidPrice = 1'999'999'900;
    static constexpr std::uint32_t InvalidHiPrice = 1'999'990'000;
    static constexpr std::uint32_t InvalidLoPrice = 1;

    std::uint16_t locate = 0;
    char name[8];
    tsbook book;

    std::uint32_t hi_price = InvalidLoPrice;
    std::uint32_t lo_price = InvalidHiPrice;
    std::uint32_t num_trades = 0;
    std::uint32_t trade_qty = 0;
    std::uint32_t num_orders = 0;

    instrument() noexcept;
    instrument(std::uint16_t locate, char const (&name)[8]) noexcept;
    void set_name(char const (&name)[8]) noexcept;

    // stats
    std::string stats_str() const noexcept;
    static std::string stats_csv_header() noexcept;
    std::string stats_csv() const noexcept;
    std::size_t allocator_stats() const noexcept;
};

/**********************************************************************/

constexpr inline double
to_hr_price(std::uint32_t p)
{
    return (p == instrument::InvalidHiPrice || p == instrument::InvalidLoPrice)
            ? 0.0
            : static_cast<double>(p) / 10000.0;
}
