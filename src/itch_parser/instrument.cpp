#include "instrument.hpp"
#include "core.hpp"
#include <fmt/format.h>


instrument::instrument() noexcept
        : locate(0)
        , name{}
        , book()
        , lo_price(InvalidLoPrice)
        , hi_price(InvalidHiPrice)
        , num_trades(0)
        , trade_qty(0)
        , num_orders(0)
{
    // empty
}


instrument::instrument(std::uint16_t l, char const (&nm)[8]) noexcept
        : locate(l)
        , name()
        , book()
        , lo_price(InvalidLoPrice)
        , hi_price(InvalidHiPrice)
        , num_trades(0)
        , trade_qty(0)
        , num_orders(0)
{
    set_name(nm);
}


void
instrument::set_name(char const (&nm)[8]) noexcept
{
    for (std::size_t i = 0; i < sizeof(nm); ++i) {
        if (nm[i] == ' ') {
            name[i] = '\0';
            break;
        }
        name[i] = nm[i];
    }
}

std::string
instrument::stats_str() const noexcept
{
    return fmt::format("{}\n"
                       "  locate:    {}\n"
                       "  low:       {:.4f}\n"
                       "  high:      {:.4f}\n"
                       "  trades:    {}\n"
                       "  trade vol: {}\n"
                       "  orders:    {}\n",
            name, locate, to_hr_price(lo_price), to_hr_price(hi_price), num_trades, trade_qty,
            num_orders);
}

std::string
instrument::stats_csv_header() noexcept
{
    return "name,locate,lo_price,hi_price,num_trades,trade_vol,num_orders";
}

std::string
instrument::stats_csv() const noexcept
{
    return fmt::format("{},{},{:.4f},{:.4f},{},{},{}", name, locate, to_hr_price(lo_price),
            to_hr_price(hi_price), num_trades, trade_qty, num_orders);
}

std::size_t
instrument::allocator_stats() const noexcept
{
    return std::max(book.bid_pool_capacity(), book.ask_pool_capacity());
}
