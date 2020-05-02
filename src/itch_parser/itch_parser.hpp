#pragma once

#include "common/instrument.hpp"
#include "protocol/itch-fmt.hpp"
#include "protocol/itch.hpp"
#include <endian.h>
#include <filesystem>
#include <algorithm> // std::max, std::min
#include <cstdio> // std::fclose, std::fopen


class itch_parser
{
private:
    struct stats
    {
        std::size_t msg_count = 0;
        std::size_t add_count = 0;
        std::size_t cancel_count = 0;
        std::size_t delete_count = 0;
        std::size_t executed_count = 0;
        std::size_t trade_count = 0;
    };

private:
    instrument instruments_[9000];
    bool logging_enabled_ = false;
    FILE* log_ = nullptr;
    std::filesystem::path stats_filepath_;
    FILE* stats_file_ = nullptr;
    stats stats_;

public:
    itch_parser(bool log, std::filesystem::path const& stats_file) noexcept;
    ~itch_parser() noexcept;
    std::size_t parse(std::uint8_t const* buf, std::size_t bytes_to_read) noexcept;
    void print_stats() const noexcept;

private:
    void handle_add_order(itch::add_order const*) noexcept;
    void handle_add_order_with_mpid(itch::add_order_with_mpid const*) noexcept;
    void handle_broken_trade(itch::broken_trade const*) noexcept;
    void handle_ipo_quoting_period_update(itch::ipo_quoting_period_update const*) noexcept;
    void handle_luld_auction_collar(itch::luld_auction_collar const*) noexcept;
    void handle_market_participant_position(itch::market_participant_position const*) noexcept;
    void handle_mwcb_decline_level(itch::mwcb_decline_level const*) noexcept;
    void handle_mwcb_status(itch::mwcb_status const*) noexcept;
    void handle_noii(itch::noii const*) noexcept;
    void handle_operational_halt(itch::operational_halt const*) noexcept;
    void handle_order_cancel(itch::order_cancel const*) noexcept;
    void handle_order_delete(itch::order_delete const*) noexcept;
    void handle_order_executed(itch::order_executed const*) noexcept;
    void handle_order_executed_with_price(itch::order_executed_with_price const*) noexcept;
    void handle_order_replace(itch::order_replace const*) noexcept;
    void handle_reg_sho_restriction(itch::reg_sho_restriction const*) noexcept;
    void handle_stock_directory(itch::stock_directory const*) noexcept;
    void handle_system_event(itch::system_event const*) noexcept;
    void handle_stock_trading_action(itch::stock_trading_action const*) noexcept;
    void handle_trade_non_cross(itch::trade_non_cross const*) noexcept;
    void handle_trade_cross(itch::trade_cross const*) noexcept;
};

/**********************************************************************/

itch_parser::itch_parser(bool enable_logging, std::filesystem::path const& stats_fp) noexcept
        : instruments_()
        , logging_enabled_(enable_logging)
        , log_(nullptr)
        , stats_filepath_(stats_fp)
        , stats_file_(nullptr)
        , stats_()
{
    if (logging_enabled_)
        log_ = std::fopen("itch.log", "w");
    if (!stats_filepath_.empty())
        stats_file_ = std::fopen(stats_filepath_.c_str(), "w");
}

itch_parser::~itch_parser() noexcept
{
    if (log_ != nullptr) {
        std::fclose(log_);
        log_ = nullptr;
    }

    if (stats_file_ != nullptr) {
        std::fclose(stats_file_);
        stats_file_ = nullptr;
    }
}

std::size_t
itch_parser::parse(std::uint8_t const* buf, std::size_t bytes_to_read) noexcept
{
    using namespace itch;

    std::size_t bytes_processed = 0;
    std::uint8_t const* end = buf + bytes_to_read;
    while (buf + sizeof(header) < end) {
        header const* hdr = reinterpret_cast<decltype(hdr)>(buf);
        std::uint16_t const msg_len = be16toh(hdr->length) + sizeof(hdr->length);

        // not enough bytes for the full msg, read nothing
        if (buf + msg_len > end)
            break;

        // clang-format off
        switch (hdr->message_type) {
            case 'A': handle_add_order(reinterpret_cast<add_order const*>(hdr)); break;
            case 'D': handle_order_delete(reinterpret_cast<order_delete const*>(hdr)); break;
            case 'U': handle_order_replace(reinterpret_cast<order_replace const*>(hdr)); break;
            case 'E': handle_order_executed(reinterpret_cast<order_executed const*>(hdr)); break;
            case 'X': handle_order_cancel(reinterpret_cast<order_cancel const*>(hdr)); break;
            case 'I': handle_noii(reinterpret_cast<noii const*>(hdr)); break;
            case 'F': handle_add_order_with_mpid(reinterpret_cast<add_order_with_mpid const*>(hdr)); break;
            case 'P': handle_trade_non_cross(reinterpret_cast<trade_non_cross const*>(hdr)); break;
            case 'L': handle_market_participant_position(reinterpret_cast<market_participant_position const*>(hdr)); break;
            case 'C': handle_order_executed_with_price(reinterpret_cast<order_executed_with_price const*>(hdr)); break;
            case 'Q': handle_trade_cross(reinterpret_cast<trade_cross const*>(hdr)); break;
            case 'Y': handle_reg_sho_restriction(reinterpret_cast<reg_sho_restriction const*>(hdr)); break;
            case 'H': handle_stock_trading_action(reinterpret_cast<stock_trading_action const*>(hdr)); break;
            case 'R': handle_stock_directory(reinterpret_cast<stock_directory const*>(hdr)); break;
            case 'S': handle_system_event(reinterpret_cast<system_event const*>(hdr)); break;
            case 'J': handle_luld_auction_collar(reinterpret_cast<luld_auction_collar const*>(hdr)); break;
            case 'K': handle_ipo_quoting_period_update(reinterpret_cast<ipo_quoting_period_update const*>(hdr)); break;
            case 'V': handle_mwcb_decline_level(reinterpret_cast<mwcb_decline_level const*>(hdr)); break;

            case 'W': handle_mwcb_status(reinterpret_cast<mwcb_status const*>(hdr)); break;
            case 'h': handle_operational_halt(reinterpret_cast<operational_halt const*>(hdr)); break;
            case 'B': handle_broken_trade(reinterpret_cast<broken_trade const*>(hdr)); break;
                // clang-format on

            default:
                fmt::print(
                        stderr, "[ERROR] parse_itch(): unknown type=[{:c}]\n", hdr->message_type);
                std::exit(1);
                break;
        }

        ++stats_.msg_count;
        buf += msg_len;
        bytes_processed += msg_len;
    }
    return bytes_processed;
}

void
itch_parser::print_stats() const noexcept
{
    // clang-format off
    fmt::print("parser stats\n"
               "  msgs processed:  {}\n"
               "  add_orders:      {}\n"
               "  cancel_orders:   {}\n"
               "  delete_orders:   {}\n"
               "  executed_orders: {}\n"
               "  trades:          {}\n",
        stats_.msg_count,
        stats_.add_count,
        stats_.cancel_count,
        stats_.delete_count,
        stats_.executed_count,
        stats_.trade_count);
    // clang-format on

    if (stats_file_ != nullptr) {
        fmt::print(stats_file_, "{}\n", instrument::stats_csv_header());

        int const num_inst = sizeof(instruments_) / sizeof(instruments_[0]);
        for (int i = 0; i < num_inst; ++i) {
            if (instruments_[i].locate == 0)
                continue;

            fmt::print(stats_file_, "{}\n", instruments_[i].stats_csv());
        }
    }
}

void
itch_parser::handle_add_order(itch::add_order const* m) noexcept
{
    if (logging_enabled_)
        fmt::print(log_, "{}\n", *m);
    ++stats_.add_count;
    std::uint16_t const index = be16toh(m->stock_locate);
    std::uint64_t const order_number = be64toh(m->order_reference_number);
    std::uint32_t const price = be32toh(m->price);
    std::uint32_t const qty = be32toh(m->shares);

    instruments_[index].book.add_order(
            order_number, m->buy_sell_indicator == 'B' ? Side::Bid : Side::Ask, price, qty);
    ++instruments_[index].num_orders;
    instruments_[index].lo_price = std::min(instruments_[index].lo_price, price);
    instruments_[index].hi_price = std::max(instruments_[index].hi_price, price);
}

void
itch_parser::handle_add_order_with_mpid(itch::add_order_with_mpid const* m) noexcept
{
    if (logging_enabled_)
        fmt::print(log_, "{}\n", *m);
    ++stats_.add_count;
    std::uint16_t const index = be16toh(m->stock_locate);
    std::uint64_t const order_number = be64toh(m->order_reference_number);
    std::uint32_t const price = be32toh(m->price);
    std::uint32_t const qty = be32toh(m->shares);

    instruments_[index].book.add_order(
            order_number, m->buy_sell_indicator == 'B' ? Side::Bid : Side::Ask, price, qty);

    ++instruments_[index].num_orders;
    instruments_[index].lo_price = std::min(instruments_[index].lo_price, price);
    instruments_[index].hi_price = std::max(instruments_[index].hi_price, price);
}

void
itch_parser::handle_broken_trade(itch::broken_trade const* m) noexcept
{
    if (logging_enabled_)
        fmt::print(log_, "{}\n", *m);
}

void
itch_parser::handle_ipo_quoting_period_update(itch::ipo_quoting_period_update const* m) noexcept
{
    if (logging_enabled_)
        fmt::print(log_, "{}\n", *m);
}

void
itch_parser::handle_luld_auction_collar(itch::luld_auction_collar const* m) noexcept
{
    if (logging_enabled_)
        fmt::print(log_, "{}\n", *m);
}

void
itch_parser::handle_market_participant_position(itch::market_participant_position const* m) noexcept
{
    if (logging_enabled_)
        fmt::print(log_, "{}\n", *m);
}

void
itch_parser::handle_mwcb_decline_level(itch::mwcb_decline_level const* m) noexcept
{
    if (logging_enabled_)
        fmt::print(log_, "{}\n", *m);
}

void
itch_parser::handle_mwcb_status(itch::mwcb_status const* m) noexcept
{
    if (logging_enabled_)
        fmt::print(log_, "{}\n", *m);
}

void
itch_parser::handle_noii(itch::noii const* m) noexcept
{
    if (logging_enabled_)
        fmt::print(log_, "{}\n", *m);
}

void
itch_parser::handle_operational_halt(itch::operational_halt const* m) noexcept
{
    if (logging_enabled_)
        fmt::print(log_, "{}\n", *m);
}

void
itch_parser::handle_order_cancel(itch::order_cancel const* m) noexcept
{
    if (logging_enabled_)
        fmt::print(log_, "{}\n", *m);
    ++stats_.cancel_count;
    std::uint16_t const index = be16toh(m->stock_locate);
    std::uint64_t const order_number = be64toh(m->order_reference_number);
    std::uint32_t const cancelled_shares = be32toh(m->cancelled_shares);

    instruments_[index].book.cancel_order(order_number, cancelled_shares);
}

void
itch_parser::handle_order_delete(itch::order_delete const* m) noexcept
{
    if (logging_enabled_)
        fmt::print(log_, "{}\n", *m);
    ++stats_.delete_count;
    std::uint16_t const index = be16toh(m->stock_locate);
    std::uint64_t const order_number = be64toh(m->order_reference_number);

    instruments_[index].book.delete_order(order_number);
}

void
itch_parser::handle_order_executed(itch::order_executed const* m) noexcept
{
    if (logging_enabled_)
        fmt::print(log_, "{}\n", *m);
    ++stats_.executed_count;
    std::uint16_t const index = be16toh(m->stock_locate);
    std::uint64_t const order_number = be64toh(m->order_reference_number);
    std::uint32_t const executed_qty = be32toh(m->executed_shares);

    instruments_[index].book.cancel_order(order_number, executed_qty);
}

void
itch_parser::handle_order_executed_with_price(itch::order_executed_with_price const* m) noexcept
{
    if (logging_enabled_)
        fmt::print(log_, "{}\n", *m);
    ++stats_.executed_count;
    std::uint16_t const index = be16toh(m->stock_locate);
    std::uint64_t const order_number = be64toh(m->order_reference_number);
    std::uint32_t const executed_qty = be32toh(m->executed_shares);

    instruments_[index].book.cancel_order(order_number, executed_qty);
}

void
itch_parser::handle_order_replace(itch::order_replace const* m) noexcept
{
    if (logging_enabled_)
        fmt::print(log_, "{}\n", *m);
    std::uint16_t const index = be16toh(m->stock_locate);
    std::uint64_t const orig_order_number = be64toh(m->original_order_reference_number);
    std::uint64_t const new_order_number = be64toh(m->new_order_reference_number);
    std::uint32_t const price = be32toh(m->price);
    std::uint32_t const qty = be32toh(m->shares);

    instruments_[index].book.replace_order(orig_order_number, new_order_number, price, qty);
}

void
itch_parser::handle_reg_sho_restriction(itch::reg_sho_restriction const* m) noexcept
{
    if (logging_enabled_)
        fmt::print(log_, "{}\n", *m);
}

void
itch_parser::handle_stock_directory(itch::stock_directory const* m) noexcept
{
    if (logging_enabled_)
        fmt::print(log_, "{}\n", *m);
    std::uint16_t const index = be16toh(m->stock_locate);
    instruments_[index].locate = index;
    instruments_[index].set_name(m->stock);
}

void
itch_parser::handle_system_event(itch::system_event const* m) noexcept
{
    if (logging_enabled_)
        fmt::print(log_, "{}\n", *m);
}

void
itch_parser::handle_stock_trading_action(itch::stock_trading_action const* m) noexcept
{
    if (logging_enabled_)
        fmt::print(log_, "{}\n", *m);
}

void
itch_parser::handle_trade_non_cross(itch::trade_non_cross const* m) noexcept
{
    if (logging_enabled_)
        fmt::print(log_, "{}\n", *m);

    std::uint16_t const index = be16toh(m->stock_locate);
    std::uint32_t const qty = be32toh(m->shares);
    ++instruments_[index].num_trades;
    instruments_[index].trade_qty += qty;
    ++stats_.trade_count;
}

void
itch_parser::handle_trade_cross(itch::trade_cross const* m) noexcept
{
    if (logging_enabled_)
        fmt::print(log_, "{}\n", *m);

    std::uint16_t const index = be16toh(m->stock_locate);
    std::uint32_t const qty = be32toh(m->shares);
    ++instruments_[index].num_trades;
    instruments_[index].trade_qty += qty;
    ++stats_.trade_count;
}
