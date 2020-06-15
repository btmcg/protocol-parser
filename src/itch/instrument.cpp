#include "instrument.hpp"
#include <fmt/format.h>
#include <algorithm> // std::transform


namespace itch {

    instrument::instrument() noexcept
            : lo(InvalidLoPrice)
            , hi(InvalidHiPrice)
    {
        // empty
    }

    instrument::instrument(std::uint16_t l, char const (&nm)[NameLen]) noexcept
            : locate(l)
            , lo(InvalidLoPrice)
            , hi(InvalidHiPrice)
    {
        set_name(nm);
    }

    void
    instrument::set_name(char const (&nm)[NameLen]) noexcept
    {
        std::transform(std::begin(nm), std::end(nm), std::begin(name),
                [](char c) { return c == ' ' ? '\0' : c; });
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
        return "name,locate,open,close,low,high,num_trades,trade_vol,num_orders,max_bids,max_asks";
    }

    std::string
    instrument::stats_csv() const
    {
        // clang-format off
        return fmt::format("{},{},{:.4f},{:.4f},{:.4f},{:.4f},{},{},{},{},{}",
            name,
            locate,
            to_hr_price(open),
            to_hr_price(close),
            to_hr_price(lo),
            to_hr_price(hi),
            num_trades,
            trade_qty,
            num_orders,
            book.max_bid_pool_used(),
            book.max_ask_pool_used());
        // clang-format on
    }

    std::tuple<std::size_t, std::size_t>
    instrument::allocator_stats() const noexcept
    {
        return std::make_tuple(book.max_bid_pool_used(), book.max_ask_pool_used());
    }

} // namespace itch
