#include "instrument.hpp"
#include <fmt/format.h>


instrument::instrument() noexcept
        : locate(0)
        , name{}
        , book()
        , hi_price(0)
        , lo_price(std::numeric_limits<std::uint32_t>::max())
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
        , hi_price(0)
        , lo_price(std::numeric_limits<std::uint32_t>::max())
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
                       "  high:      {}\n"
                       "  low:       {}\n"
                       "  trades:    {}\n"
                       "  trade vol: {}\n"
                       "  orders:    {}\n",
            name, locate, hi_price, lo_price, num_trades, trade_qty, num_orders);
}

std::string
instrument::stats_csv() const noexcept
{
    return fmt::format("{},{},{},{},{},{},{}", name, locate, hi_price, lo_price, num_trades,
            trade_qty, num_orders);
}
