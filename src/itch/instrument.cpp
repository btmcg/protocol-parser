#include "instrument.hpp"
#include <fmt/format.h>


namespace itch {

    instrument::instrument() noexcept
            : locate(0)
            , name{}
            , book()
            , open(0)
            , close(0)
            , lo(InvalidLoPrice)
            , hi(InvalidHiPrice)
            , num_trades(0)
            , trade_qty(0)
            , num_orders(0)
            , instrument_state(InstrumentState::Unknown)
    {
        // empty
    }

    instrument::instrument(std::uint16_t l, char const (&nm)[NameLen]) noexcept
            : locate(l)
            , name()
            , book()
            , open(0)
            , close(0)
            , lo(InvalidLoPrice)
            , hi(InvalidHiPrice)
            , num_trades(0)
            , trade_qty(0)
            , num_orders(0)
            , instrument_state(InstrumentState::Unknown)
    {
        set_name(nm);
    }

    void
    instrument::set_name(char const (&nm)[NameLen]) noexcept
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
    instrument::stats_str() const
    {
        // clang-format off
        return fmt::format("{}\n"
                        "  locate:    {}\n"
                        "  open:      {:.4f}\n"
                        "  close:     {:.4f}\n"
                        "  low:       {:.4f}\n"
                        "  high:      {:.4f}\n"
                        "  trades:    {}\n"
                        "  trade vol: {}\n"
                        "  orders:    {}\n",
            name,
            locate,
            to_hr_price(open),
            to_hr_price(close),
            to_hr_price(lo),
            to_hr_price(hi),
            num_trades,
            trade_qty,
            num_orders);
        // clang-format on
    }

    std::string
    instrument::stats_csv_header() noexcept
    {
        return "name,locate,open,close,low,high,num_trades,trade_vol,num_orders";
    }

    std::string
    instrument::stats_csv() const
    {
        // clang-format off
        return fmt::format("{},{},{:.4f},{:.4f},{:.4f},{:.4f},{},{},{}",
            name,
            locate,
            to_hr_price(open),
            to_hr_price(close),
            to_hr_price(lo),
            to_hr_price(hi),
            num_trades,
            trade_qty,
            num_orders);
        // clang-format on
    }

    std::pair<std::size_t, std::size_t>
    instrument::allocator_stats() const noexcept
    {
        return std::make_pair(book.max_bid_pool_used(), book.max_ask_pool_used());
    }

} // namespace itch
