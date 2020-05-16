#include "instrument.hpp"
#include <fmt/format.h>


instrument::instrument() noexcept
        : locate(0)
        , name{}
        , book()
        , hi_price(InvalidHiPrice)
        , lo_price(InvalidLoPrice)
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
        , hi_price(InvalidHiPrice)
        , lo_price(InvalidLoPrice)
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
                       "  high:      {:.4f}\n"
                       "  low:       {:.4f}\n"
                       "  trades:    {}\n"
                       "  trade vol: {}\n"
                       "  orders:    {}\n",
            name, locate, to_hr_price(hi_price), to_hr_price(lo_price), num_trades, trade_qty,
            num_orders);
}

std::string
instrument::stats_csv_header() noexcept
{
    return "name,locate,high_price,low_price,num_trades,trade_vol,num_orders";
}

std::string
instrument::stats_csv() const noexcept
{
    return fmt::format("{},{},{:.4f},{:.4f},{},{},{}", name, locate, to_hr_price(hi_price),
            to_hr_price(lo_price), num_trades, trade_qty, num_orders);
}

std::size_t
instrument::allocator_stats() const noexcept
{
    return 0;
    // return std::max(book.bids().get_allocator().max_size_reached(),
    //         book.asks().get_allocator().max_size_reached());
}
