#pragma once

#include "core.hpp"
#include "instrument.hpp"
#include "protocol/itch-fmt.hpp"
#include "protocol/itch.hpp"
#include <endian.h>
#include <filesystem>
#include <algorithm> // std::max, std::min
#include <cstddef> // std::size_t
#include <cstdint>
#include <cstdio> // std::fclose, std::fopen
#include <vector>


namespace itch {

    class parser
    {
    private:
        enum
        {
            /// the number of distinct instruments cannot exceed this number
            MaxNumInstruments = 9000,

            /// the number of distinct orders per run cannot exceed this number
            MaxNumOrders = 800'000'000
        };

    private:
        struct msg_stats
        {
            std::size_t msg_count = 0;
        };

    private:
        instrument instruments_[MaxNumInstruments];
        std::vector<order> orders_;
        MarketState market_state_ = MarketState::Unknown;
        FILE* stats_file_ = nullptr;
        msg_stats msg_stats_;
        std::filesystem::path stats_filepath_;
        bool logging_enabled_ = false;
        FILE* log_ = nullptr;

    public:
        parser(bool log, std::filesystem::path const& stats_file) noexcept;
        ~parser() noexcept;
        std::size_t parse(std::uint8_t const* buf, std::size_t bytes_to_read) noexcept;
        void print_stats() const noexcept;

    private:
        void handle_add_order(add_order const*) noexcept;
        void handle_add_order_with_mpid(add_order_with_mpid const*) noexcept;
        void handle_broken_trade(broken_trade const*) noexcept;
        void handle_ipo_quoting_period_update(ipo_quoting_period_update const*) noexcept;
        void handle_luld_auction_collar(luld_auction_collar const*) noexcept;
        void handle_market_participant_position(market_participant_position const*) noexcept;
        void handle_mwcb_decline_level(mwcb_decline_level const*) noexcept;
        void handle_mwcb_status(mwcb_status const*) noexcept;
        void handle_noii(noii const*) noexcept;
        void handle_operational_halt(operational_halt const*) noexcept;
        void handle_order_cancel(order_cancel const*) noexcept;
        void handle_order_delete(order_delete const*) noexcept;
        void handle_order_executed(order_executed const*) noexcept;
        void handle_order_executed_with_price(order_executed_with_price const*) noexcept;
        void handle_order_replace(order_replace const*) noexcept;
        void handle_reg_sho_restriction(reg_sho_restriction const*) noexcept;
        void handle_stock_directory(stock_directory const*) noexcept;
        void handle_stock_trading_action(stock_trading_action const*) noexcept;
        void handle_system_event(system_event const*) noexcept;
        void handle_trade_non_cross(trade_non_cross const*) noexcept;
        void handle_trade_cross(trade_cross const*) noexcept;
    };

    /**********************************************************************/

    parser::parser(bool enable_logging, std::filesystem::path const& stats_fp) noexcept
            : instruments_()
            , orders_(MaxNumOrders, order())
            , market_state_(MarketState::Unknown)
            , stats_file_(nullptr)
            , msg_stats_()
            , stats_filepath_(stats_fp)
            , logging_enabled_(enable_logging)
            , log_(nullptr)
    {
        if (logging_enabled_)
            log_ = std::fopen("itch.log", "w");
        if (!stats_filepath_.empty())
            stats_file_ = std::fopen(stats_filepath_.c_str(), "w");
    }

    parser::~parser() noexcept
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
    parser::parse(std::uint8_t const* buf, std::size_t bytes_to_read) noexcept
    {
        std::size_t bytes_processed = 0;
        std::uint8_t const* end = buf + bytes_to_read;
        while (buf + sizeof(header) < end) {
            header const* hdr = reinterpret_cast<decltype(hdr)>(buf);
            std::uint16_t const msg_len = be16toh(hdr->length) + sizeof(hdr->length);

            // not enough bytes for the full msg, read nothing
            if (buf + msg_len > end)
                break;

            // clang-format off
            // switch ordered by msg count on sample day (2020-01-30)
            switch (hdr->msg_type) {
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

                default:
                    fmt::print(stderr, "[ERROR] parse_itch(): unknown type=[{:c}]\n", hdr->msg_type);
                    std::abort();
                    break;
            }
            // clang-format on

            ++msg_stats_.msg_count;
            buf += msg_len;
            bytes_processed += msg_len;
        }
        return bytes_processed;
    }

    void
    parser::print_stats() const noexcept
    {
        std::size_t max_bid_pool_used = 0;
        std::size_t max_ask_pool_used = 0;
        for (auto const& itr : instruments_) {
            max_bid_pool_used = std::max(max_bid_pool_used, itr.allocator_stats().first);
            max_ask_pool_used = std::max(max_ask_pool_used, itr.allocator_stats().second);
        }

        // clang-format off
        fmt::print("parser msg stats\n"
                "  msgs processed:    {}\n"
                "  max_bid_pool_used: {}\n"
                "  max_ask_pool_used: {}\n",
            msg_stats_.msg_count,
            max_bid_pool_used,
            max_ask_pool_used);
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
    parser::handle_add_order(add_order const* m) noexcept
    {
        if (logging_enabled_)
            fmt::print(log_, "{}\n", *m);

        std::uint16_t const index = be16toh(m->stock_locate);
        std::uint64_t const order_number = be64toh(m->order_reference_number);

        order& o = orders_[order_number];
        o.price = be32toh(m->price);
        o.qty = be32toh(m->shares);
        o.side = m->buy_sell_indicator == 'B' ? Side::Bid : Side::Ask;
        o.ts = to_local_nsecs(from_itch_timestamp(m->timestamp));

        instruments_[index].book.add_order(o);

        ++instruments_[index].num_orders;
    }

    void
    parser::handle_add_order_with_mpid(add_order_with_mpid const* m) noexcept
    {
        if (logging_enabled_)
            fmt::print(log_, "{}\n", *m);

        std::uint16_t const index = be16toh(m->stock_locate);
        std::uint64_t const order_number = be64toh(m->order_reference_number);

        order& o = orders_[order_number];
        o.price = be32toh(m->price);
        o.qty = be32toh(m->shares);
        o.side = m->buy_sell_indicator == 'B' ? Side::Bid : Side::Ask;
        o.ts = to_local_nsecs(from_itch_timestamp(m->timestamp));

        instruments_[index].book.add_order(o);

        ++instruments_[index].num_orders;
    }

    void
    parser::handle_broken_trade(broken_trade const* m) noexcept
    {
        if (logging_enabled_)
            fmt::print(log_, "{}\n", *m);
    }

    void
    parser::handle_ipo_quoting_period_update(ipo_quoting_period_update const* m) noexcept
    {
        if (logging_enabled_)
            fmt::print(log_, "{}\n", *m);
    }

    void
    parser::handle_luld_auction_collar(luld_auction_collar const* m) noexcept
    {
        if (logging_enabled_)
            fmt::print(log_, "{}\n", *m);
    }

    void
    parser::handle_market_participant_position(market_participant_position const* m) noexcept
    {
        if (logging_enabled_)
            fmt::print(log_, "{}\n", *m);
    }

    void
    parser::handle_mwcb_decline_level(mwcb_decline_level const* m) noexcept
    {
        if (logging_enabled_)
            fmt::print(log_, "{}\n", *m);
    }

    void
    parser::handle_mwcb_status(mwcb_status const* m) noexcept
    {
        if (logging_enabled_)
            fmt::print(log_, "{}\n", *m);
    }

    void
    parser::handle_noii(noii const* m) noexcept
    {
        if (logging_enabled_)
            fmt::print(log_, "{}\n", *m);
    }

    void
    parser::handle_operational_halt(operational_halt const* m) noexcept
    {
        if (logging_enabled_)
            fmt::print(log_, "{}\n", *m);

        std::uint16_t const index = be16toh(m->stock_locate);

        if (m->operational_halt_action == 'H')
            instruments_[index].instrument_state = InstrumentState::OperationalHalted;
        else if (m->operational_halt_action == 'T')
            instruments_[index].instrument_state = InstrumentState::Trading;
        else
            fmt::print(stderr, "[ERROR] received unknown operational halt action: {}\n", *m);
    }

    void
    parser::handle_order_cancel(order_cancel const* m) noexcept
    {
        if (logging_enabled_)
            fmt::print(log_, "{}\n", *m);

        std::uint16_t const index = be16toh(m->stock_locate);
        std::uint64_t const order_number = be64toh(m->order_reference_number);

        order& o = orders_[order_number];
        qty_t const cancelled_shares = be32toh(m->cancelled_shares);

        instruments_[index].book.cancel_order(o, cancelled_shares);
    }

    void
    parser::handle_order_delete(order_delete const* m) noexcept
    {
        if (logging_enabled_)
            fmt::print(log_, "{}\n", *m);

        std::uint16_t const index = be16toh(m->stock_locate);
        std::uint64_t const order_number = be64toh(m->order_reference_number);

        order& o = orders_[order_number];
        instruments_[index].book.delete_order(o);
        o.clear();
    }

    void
    parser::handle_order_executed(order_executed const* m) noexcept
    {
        if (logging_enabled_)
            fmt::print(log_, "{}\n", *m);

        std::uint16_t const index = be16toh(m->stock_locate);
        std::uint64_t const order_number = be64toh(m->order_reference_number);

        order& o = orders_[order_number];
        qty_t const executed_qty = be32toh(m->executed_shares);
        price_t const order_price = o.price;

        instruments_[index].book.cancel_order(o, executed_qty);
        instruments_[index].trade_qty += executed_qty;
        instruments_[index].last_trade_price = order_price;
        ++instruments_[index].num_trades;

        if (market_state_ == MarketState::Open) {
            // not all cross_trades marked as opening have prices, so
            // record this
            if (instruments_[index].open_price == 0)
                instruments_[index].open_price = order_price;

            if (instruments_[index].lo_price == InvalidLoPrice
                    || order_price < instruments_[index].lo_price)
                instruments_[index].lo_price = order_price;
            if (instruments_[index].hi_price == InvalidHiPrice
                    || order_price > instruments_[index].hi_price)
                instruments_[index].hi_price = order_price;
        }
    }

    void
    parser::handle_order_executed_with_price(order_executed_with_price const* m) noexcept
    {
        if (logging_enabled_)
            fmt::print(log_, "{}\n", *m);

        std::uint16_t const index = be16toh(m->stock_locate);
        std::uint64_t const order_number = be64toh(m->order_reference_number);

        order& o = orders_[order_number];
        qty_t const executed_qty = be32toh(m->executed_shares);
        price_t const executed_price = be32toh(m->execution_price);

        instruments_[index].book.cancel_order(o, executed_qty);

        // only record stats if execution is marked "printable"
        if (market_state_ == MarketState::Open && m->printable == 'Y') {
            if (instruments_[index].lo_price == InvalidLoPrice
                    || executed_price < instruments_[index].lo_price)
                instruments_[index].lo_price = executed_price;
            if (instruments_[index].hi_price == InvalidHiPrice
                    || o.price > instruments_[index].hi_price)
                instruments_[index].hi_price = executed_price;

            instruments_[index].trade_qty += executed_qty;
            instruments_[index].last_trade_price = executed_price;
            ++instruments_[index].num_trades;
        }
    }

    void
    parser::handle_order_replace(order_replace const* m) noexcept
    {
        if (logging_enabled_)
            fmt::print(log_, "{}\n", *m);

        std::uint16_t const index = be16toh(m->stock_locate);
        std::uint64_t const orig_order_number = be64toh(m->original_order_reference_number);
        std::uint64_t const new_order_number = be64toh(m->new_order_reference_number);

        order& old_order = orders_[orig_order_number];
        order& new_order = orders_[new_order_number];

        new_order.side = old_order.side;
        new_order.price = be32toh(m->price);
        new_order.qty = be32toh(m->shares);

        instruments_[index].book.replace_order(old_order, new_order);
    }

    void
    parser::handle_reg_sho_restriction(reg_sho_restriction const* m) noexcept
    {
        if (logging_enabled_)
            fmt::print(log_, "{}\n", *m);
    }

    void
    parser::handle_stock_directory(stock_directory const* m) noexcept
    {
        if (logging_enabled_)
            fmt::print(log_, "{}\n", *m);

        std::uint16_t const index = be16toh(m->stock_locate);
        instruments_[index].locate = index;
        instruments_[index].set_name(m->stock);
    }

    void
    parser::handle_stock_trading_action(stock_trading_action const* m) noexcept
    {
        if (logging_enabled_)
            fmt::print(log_, "{}\n", *m);

        std::uint16_t const index = be16toh(m->stock_locate);

        // clang-format off
        switch (m->trading_state) {
            case 'H': instruments_[index].instrument_state = InstrumentState::OperationalHalted; break;
            case 'P': instruments_[index].instrument_state = InstrumentState::Paused; break;
            case 'T': instruments_[index].instrument_state = InstrumentState::Trading; break;
            case 'Q': instruments_[index].instrument_state = InstrumentState::QuotationOnly; break;

            default: fmt::print(stderr, "[ERROR] received unknown trading action: {}\n", *m); break;
        }
        // clang-format on
    }

    void
    parser::handle_system_event(system_event const* m) noexcept
    {
        if (logging_enabled_)
            fmt::print(log_, "{}\n", *m);

        switch (m->event_code) {
            case 'O':
                fmt::print("{} system event: start of messages\n",
                        to_local_time(from_itch_timestamp(m->timestamp)));
                market_state_ = MarketState::SystemUp;
                break;

            case 'S':
                fmt::print("{} system event: system start\n",
                        to_local_time(from_itch_timestamp(m->timestamp)));
                market_state_ = MarketState::AcceptingOrders;
                break;

            case 'Q':
                fmt::print("{} system event: market open\n",
                        to_local_time(from_itch_timestamp(m->timestamp)));
                market_state_ = MarketState::Open;
                break;

            case 'M':
                fmt::print("{} system event: market close\n",
                        to_local_time(from_itch_timestamp(m->timestamp)));
                market_state_ = MarketState::Closed;

                // update close prices if needed
                for (auto& i : instruments_) {
                    if (i.close_price == 0)
                        i.close_price
                                = (i.last_trade_price == 0) ? i.open_price : i.last_trade_price;
                }
                break;

            case 'E':
                fmt::print("{} system event: system stop\n",
                        to_local_time(from_itch_timestamp(m->timestamp)));
                market_state_ = MarketState::SystemDown;
                break;

            case 'C':
                fmt::print("{} system event: end of messages\n",
                        to_local_time(from_itch_timestamp(m->timestamp)));
                market_state_ = MarketState::Unknown;
                break;

            default:
                fmt::print(
                        stderr, "[ERROR] received unknown system event code: {}\n", m->event_code);
                market_state_ = MarketState::Unknown;
                break;
        }
    }

    void
    parser::handle_trade_non_cross(trade_non_cross const* m) noexcept
    {
        if (logging_enabled_)
            fmt::print(log_, "{}\n", *m);

        std::uint16_t const index = be16toh(m->stock_locate);
        instruments_[index].trade_qty += be32toh(m->shares);
        instruments_[index].last_trade_price = be32toh(m->price);
        ++instruments_[index].num_trades;
    }

    void
    parser::handle_trade_cross(trade_cross const* m) noexcept
    {
        if (logging_enabled_)
            fmt::print(log_, "{}\n", *m);

        // trade cross msgs shouldn't be counted in market statistic
        // calculations (i think) (pg 16)
        std::uint16_t const index = be16toh(m->stock_locate);
        if (m->cross_type == 'O') {
            price_t const cp = be32toh(m->cross_price);
            if (cp != 0)
                instruments_[index].open_price = cp;
        } else if (m->cross_type == 'C') {
            price_t const cp = be32toh(m->cross_price);
            if (cp != 0)
                instruments_[index].close_price = cp;
        }
    }

} // namespace itch
