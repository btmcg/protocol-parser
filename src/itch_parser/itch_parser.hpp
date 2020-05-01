#pragma once

#include "common/instrument.hpp"
#include "protocol/itch-fmt.hpp"
#include "protocol/itch.hpp"
#include <endian.h>
#include <cstdio> // std::fclose, std::fopen


class itch_parser
{
private:
    instrument instruments_[9000];
    FILE* log_ = nullptr;

public:
    itch_parser() noexcept;
    ~itch_parser() noexcept;
    std::size_t parse(std::uint8_t const* buf, std::size_t bytes_to_read) noexcept;

private:
    void handle_add_order(itch::add_order const*) noexcept;
    void handle_add_order_with_mpid(itch::add_order_with_mpid const*) noexcept;
    void handle_noii(itch::noii const*) noexcept;
    void handle_order_cancel(itch::order_cancel const*) noexcept;
    void handle_order_delete(itch::order_delete const*) noexcept;
    void handle_order_executed(itch::order_executed const*) noexcept;
    void handle_order_replace(itch::order_replace const*) noexcept;
    void handle_stock_directory(itch::stock_directory const*) noexcept;
    void handle_trade_non_cross(itch::trade_non_cross const*) noexcept;
};

/**********************************************************************/

itch_parser::itch_parser() noexcept
        : instruments_()
        , log_(nullptr)
{
    log_ = std::fopen("itch.log", "w");
}

itch_parser::~itch_parser() noexcept
{
    std::fclose(log_);
    log_ = nullptr;
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
        //     case 'L': fmt::print(stderr, "{}\n", *reinterpret_cast<market_participant_position const*>(hdr)); break;
        //     case 'C': fmt::print(stderr, "{}\n", *reinterpret_cast<order_executed_with_price const*>(hdr)); break;
        //     case 'Q': fmt::print(stderr, "{}\n", *reinterpret_cast<trade_cross const*>(hdr)); break;
        //     case 'Y': fmt::print(stderr, "{}\n", *reinterpret_cast<reg_sho_restriction const*>(hdr)); break;
        //     case 'H': fmt::print(stderr, "{}\n", *reinterpret_cast<stock_trading_action const*>(hdr)); break;
            case 'R': handle_stock_directory(reinterpret_cast<stock_directory const*>(hdr)); break;
        //     case 'S': fmt::print(stderr, "{}\n", *reinterpret_cast<system_event const*>(hdr)); break;
        //     case 'J': fmt::print(stderr, "{}\n", *reinterpret_cast<luld_auction_collar const*>(hdr)); break;
        //     case 'K': fmt::print(stderr, "{}\n", *reinterpret_cast<ipo_quoting_period_update const*>(hdr)); break;
        //     case 'V': fmt::print(stderr, "{}\n", *reinterpret_cast<mwcb_decline_level const*>(hdr)); break;
        //
        //     case 'W': fmt::print(stderr, "{}\n", *reinterpret_cast<mwcb_status const*>(hdr)); break;
        //     case 'h': fmt::print(stderr, "{}\n", *reinterpret_cast<operational_halt const*>(hdr)); break;
        //     case 'B': fmt::print(stderr, "{}\n", *reinterpret_cast<broken_trade const*>(hdr)); break;

            // default: fmt::print(stderr, "[ERROR] parse_itch(): unknown type=[{:c}]\n", hdr->message_type); std::exit(1); break;
        }
        // clang-format on

        // ++stats_.msg_type_count[hdr->message_type];
        // stats_.byte_count += msg_len;
        // ++stats_.msg_count;
        buf += msg_len;
        bytes_processed += msg_len;
    }
    return bytes_processed;
}

void
itch_parser::handle_add_order(itch::add_order const* m) noexcept
{
    fmt::print(log_, "{}\n", *m);
    std::uint16_t const index = be16toh(m->stock_locate);
    std::uint64_t const order_number = be64toh(m->order_reference_number);
    std::uint32_t const price = be32toh(m->price);
    std::uint32_t const qty = be32toh(m->shares);

    instruments_[index].book.add_order(
            order_number, m->buy_sell_indicator == 'B' ? Side::Bid : Side::Ask, price, qty);
}

void
itch_parser::handle_noii(itch::noii const* m) noexcept
{
    fmt::print(log_, "{}\n", *m);
}

void
itch_parser::handle_order_cancel(itch::order_cancel const* m) noexcept
{
    fmt::print(log_, "{}\n", *m);
    std::uint16_t const index = be16toh(m->stock_locate);
    std::uint64_t const order_number = be64toh(m->order_reference_number);
    std::uint32_t const cancelled_shares = be32toh(m->cancelled_shares);

    instruments_[index].book.cancel_order(order_number, cancelled_shares);
}

void
itch_parser::handle_order_delete(itch::order_delete const* m) noexcept
{
    fmt::print(log_, "{}\n", *m);
    std::uint16_t const index = be16toh(m->stock_locate);
    std::uint64_t const order_number = be64toh(m->order_reference_number);

    instruments_[index].book.delete_order(order_number);
}

void
itch_parser::handle_order_executed(itch::order_executed const* m) noexcept
{
    fmt::print(log_, "{}\n", *m);
    std::uint16_t const index = be16toh(m->stock_locate);
    std::uint64_t const order_number = be64toh(m->order_reference_number);
    std::uint32_t const executed_qty = be32toh(m->executed_shares);

    instruments_[index].book.cancel_order(order_number, executed_qty);
}

void
itch_parser::handle_order_replace(itch::order_replace const* m) noexcept
{
    fmt::print(log_, "{}\n", *m);
    std::uint16_t const index = be16toh(m->stock_locate);
    std::uint64_t const orig_order_number = be64toh(m->original_order_reference_number);
    std::uint64_t const new_order_number = be64toh(m->new_order_reference_number);
    std::uint32_t const price = be32toh(m->price);
    std::uint32_t const qty = be32toh(m->shares);

    instruments_[index].book.replace_order(orig_order_number, new_order_number, price, qty);
}

void
itch_parser::handle_add_order_with_mpid(itch::add_order_with_mpid const* m) noexcept
{
    fmt::print(log_, "{}\n", *m);
    std::uint16_t const index = be16toh(m->stock_locate);
    std::uint64_t const order_number = be64toh(m->order_reference_number);
    std::uint32_t const price = be32toh(m->price);
    std::uint32_t const qty = be32toh(m->shares);

    instruments_[index].book.add_order(
            order_number, m->buy_sell_indicator == 'B' ? Side::Bid : Side::Ask, price, qty);
}

void
itch_parser::handle_stock_directory(itch::stock_directory const* m) noexcept
{
    fmt::print(log_, "{}\n", *m);
    std::uint16_t const index = be16toh(m->stock_locate);
    instruments_[index].locate = index;
    instruments_[index].set_name(m->stock);
}

void
itch_parser::handle_trade_non_cross(itch::trade_non_cross const* m) noexcept
{
    fmt::print(log_, "{}\n", *m);
}
