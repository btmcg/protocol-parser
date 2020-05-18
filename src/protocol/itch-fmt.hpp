#pragma once

#include "itch.hpp"
#include <endian.h>
#include <fmt/chrono.h>
#include <fmt/format.h>
#include <ctime>


namespace { // unnamed
    /// returns nsecs since midnight
    constexpr std::uint64_t
    from_itch_timestamp(std::uint8_t const (&timestamp)[6]) noexcept
    {
        std::uint64_t t = 0;
        for (std::size_t i = 0; i < sizeof(timestamp); ++i)
            t = (t << 8) + timestamp[i];
        return t;
    }

    /// returns nsecs (since epoch) of the last midnight
    inline std::uint64_t
    get_midnight_nsecs()
    {
        std::time_t const now = std::time(nullptr);
        tm midnight_tm;
        ::localtime_r(&now, &midnight_tm);
        midnight_tm.tm_sec = midnight_tm.tm_min = midnight_tm.tm_hour = 0;

        return std::mktime(&midnight_tm) * 1000 * 1000 * 1000;
    }

    std::uint64_t const MidnightNsec = get_midnight_nsecs();

    /// returns nsecs (since epoch) of time given
    inline std::uint64_t
    to_local_nsecs(std::uint64_t nsecs_since_midnight)
    {
        return MidnightNsec + nsecs_since_midnight;
    }

    /// returns hh:mm:ss.nnnnnnnnn of time given
    inline std::string
    to_local_time(std::uint64_t nsecs_since_midnight)
    {
        std::uint64_t const now_nsec = to_local_nsecs(nsecs_since_midnight);
        std::time_t const now = now_nsec / 1000 / 1000 / 1000;
        std::uint64_t const nsec = now_nsec % now;

        return fmt::format("{:%H:%M:%S}.{:0>9}", *std::localtime(&now), nsec);
    }
} // namespace


template <>
struct fmt::formatter<itch::system_event>
{
    constexpr auto
    parse(format_parse_context& ctx)
    {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto
    format(itch::system_event const& m, FormatContext& ctx)
    {
        return format_to(ctx.out(),
                "system_event(length={},msg_type={},stock_locate={},tracking_number={},timestamp={},event_code={})",
                be16toh(m.length), m.msg_type, be16toh(m.stock_locate), be16toh(m.tracking_number),
                to_local_time(from_itch_timestamp(m.timestamp)), m.event_code);
    }
};

template <>
struct fmt::formatter<itch::stock_directory>
{
    constexpr auto
    parse(format_parse_context& ctx)
    {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto
    format(itch::stock_directory const& m, FormatContext& ctx)
    {
        return format_to(ctx.out(),
                "stock_directory(length={},msg_type={},stock_locate={},tracking_number={},timestamp={},stock={:.{}},market_category={},financial_status_indicator={},round_lot_size={},round_lots_only={},issue_classification={},issue_subtype={:.{}},authenticity={},short_sale_threshold_indicator={},ipo_flag={},luld_reference_price_tier={},etp_flag={},etp_leverage_factor={},inverse_indicator={})",
                be16toh(m.length), m.msg_type, be16toh(m.stock_locate), be16toh(m.tracking_number),
                to_local_time(from_itch_timestamp(m.timestamp)), m.stock, sizeof(m.stock),
                m.market_category, m.financial_status_indicator, be32toh(m.round_lot_size),
                m.round_lots_only, m.issue_classification, m.issue_subtype, sizeof(m.issue_subtype),
                m.authenticity, m.short_sale_threshold_indicator, m.ipo_flag,
                m.luld_reference_price_tier, m.etp_flag, be32toh(m.etp_leverage_factor),
                m.inverse_indicator);
    }
};

template <>
struct fmt::formatter<itch::stock_trading_action>
{
    constexpr auto
    parse(format_parse_context& ctx)
    {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto
    format(itch::stock_trading_action const& m, FormatContext& ctx)
    {
        return format_to(ctx.out(),
                "stock_trading_action(length={},msg_type={},stock_locate={},tracking_number={},timestamp={},stock={:.{}},trading_state={},reserved={},reason={:.{}})",
                be16toh(m.length), m.msg_type, be16toh(m.stock_locate), be16toh(m.tracking_number),
                to_local_time(from_itch_timestamp(m.timestamp)), m.stock, sizeof(m.stock),
                m.trading_state, m.reserved, m.reason, sizeof(m.reason));
    }
};

template <>
struct fmt::formatter<itch::reg_sho_restriction>
{
    constexpr auto
    parse(format_parse_context& ctx)
    {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto
    format(itch::reg_sho_restriction const& m, FormatContext& ctx)
    {
        return format_to(ctx.out(),
                "reg_sho_restriction(length={},msg_type={},stock_locate={},tracking_number={},timestamp={},stock={:.{}},reg_sho_action={})",
                be16toh(m.length), m.msg_type, be16toh(m.stock_locate), be16toh(m.tracking_number),
                to_local_time(from_itch_timestamp(m.timestamp)), m.stock, sizeof(m.stock),
                m.reg_sho_action);
    }
};

template <>
struct fmt::formatter<itch::market_participant_position>
{
    constexpr auto
    parse(format_parse_context& ctx)
    {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto
    format(itch::market_participant_position const& m, FormatContext& ctx)
    {
        return format_to(ctx.out(),
                "market_participant_position(length={},msg_type={},stock_locate={},tracking_number={},timestamp={},mpid={:.{}},stock={:.{}},primary_market_maker={},market_maker_mode={},market_participant_state={})",
                be16toh(m.length), m.msg_type, be16toh(m.stock_locate), be16toh(m.tracking_number),
                to_local_time(from_itch_timestamp(m.timestamp)), m.mpid, sizeof(m.mpid), m.stock,
                sizeof(m.stock), m.primary_market_maker, m.market_maker_mode,
                m.market_participant_state);
    }
};

template <>
struct fmt::formatter<itch::mwcb_decline_level>
{
    constexpr auto
    parse(format_parse_context& ctx)
    {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto
    format(itch::mwcb_decline_level const& m, FormatContext& ctx)
    {
        return format_to(ctx.out(),
                "mwcb_decline_level(length={},msg_type={},stock_locate={},tracking_number={},timestamp={},level1={},level2={},level3={})",
                be16toh(m.length), m.msg_type, be16toh(m.stock_locate), be16toh(m.tracking_number),
                to_local_time(from_itch_timestamp(m.timestamp)), be64toh(m.level1),
                be64toh(m.level2), be64toh(m.level3));
    }
};

template <>
struct fmt::formatter<itch::mwcb_status>
{
    constexpr auto
    parse(format_parse_context& ctx)
    {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto
    format(itch::mwcb_status const& m, FormatContext& ctx)
    {
        return format_to(ctx.out(),
                "mwcb_status(length={},msg_type={},stock_locate={},tracking_number={},timestamp={},breached_level={})",
                be16toh(m.length), m.msg_type, be16toh(m.stock_locate), be16toh(m.tracking_number),
                to_local_time(from_itch_timestamp(m.timestamp)), m.breached_level);
    }
};

template <>
struct fmt::formatter<itch::ipo_quoting_period_update>
{
    constexpr auto
    parse(format_parse_context& ctx)
    {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto
    format(itch::ipo_quoting_period_update const& m, FormatContext& ctx)
    {
        return format_to(ctx.out(),
                "ipo_quoting_period_update(length={},msg_type={},stock_locate={},tracking_number={},timestamp={},stock={:.{}},ipo_quotation_release_time={},ipo_quotation_release_qualifier={},ipo_price={})",
                be16toh(m.length), m.msg_type, be16toh(m.stock_locate), be16toh(m.tracking_number),
                to_local_time(from_itch_timestamp(m.timestamp)), m.stock, sizeof(m.stock),
                be32toh(m.ipo_quotation_release_time), m.ipo_quotation_release_qualifier,
                be32toh(m.ipo_price));
    }
};

template <>
struct fmt::formatter<itch::luld_auction_collar>
{
    constexpr auto
    parse(format_parse_context& ctx)
    {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto
    format(itch::luld_auction_collar const& m, FormatContext& ctx)
    {
        return format_to(ctx.out(),
                "luld_auction_collar(length={},msg_type={},stock_locate={},tracking_number={},timestamp={},stock={:.{}},auction_collar_reference_price={},upper_auction_collar_price={},lower_auction_collar_price={},auction_collar_extension={})",
                be16toh(m.length), m.msg_type, be16toh(m.stock_locate), be16toh(m.tracking_number),
                to_local_time(from_itch_timestamp(m.timestamp)), m.stock, sizeof(m.stock),
                be32toh(m.auction_collar_reference_price), be32toh(m.upper_auction_collar_price),
                be32toh(m.lower_auction_collar_price), be32toh(m.auction_collar_extension));
    }
};

template <>
struct fmt::formatter<itch::operational_halt>
{
    constexpr auto
    parse(format_parse_context& ctx)
    {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto
    format(itch::operational_halt const& m, FormatContext& ctx)
    {
        return format_to(ctx.out(),
                "operational_halt(length={},msg_type={},stock_locate={},tracking_number={},timestamp={},stock={:.{}},market_code={},operational_halt_action={})",
                be16toh(m.length), m.msg_type, be16toh(m.stock_locate), be16toh(m.tracking_number),
                to_local_time(from_itch_timestamp(m.timestamp)), m.stock, sizeof(m.stock),
                m.market_code, m.operational_halt_action);
    }
};

template <>
struct fmt::formatter<itch::add_order>
{
    constexpr auto
    parse(format_parse_context& ctx)
    {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto
    format(itch::add_order const& m, FormatContext& ctx)
    {
        return format_to(ctx.out(),
                "add_order(length={},msg_type={},stock_locate={},tracking_number={},timestamp={},order_reference_number={},buy_sell_indicator={},shares={},stock={:.{}},price={})",
                be16toh(m.length), m.msg_type, be16toh(m.stock_locate), be16toh(m.tracking_number),
                to_local_time(from_itch_timestamp(m.timestamp)), be64toh(m.order_reference_number),
                m.buy_sell_indicator, be32toh(m.shares), m.stock, sizeof(m.stock),
                be32toh(m.price));
    }
};

template <>
struct fmt::formatter<itch::add_order_with_mpid>
{
    constexpr auto
    parse(format_parse_context& ctx)
    {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto
    format(itch::add_order_with_mpid const& m, FormatContext& ctx)
    {
        return format_to(ctx.out(),
                "add_order_with_mpid(length={},msg_type={},stock_locate={},tracking_number={},timestamp={},order_reference_number={},buy_sell_indicator={},shares={},stock={:.{}},price={},attribution={:.{}})",
                be16toh(m.length), m.msg_type, be16toh(m.stock_locate), be16toh(m.tracking_number),
                to_local_time(from_itch_timestamp(m.timestamp)), be64toh(m.order_reference_number),
                m.buy_sell_indicator, be32toh(m.shares), m.stock, sizeof(m.stock), be32toh(m.price),
                m.attribution, sizeof(m.attribution));
    }
};

template <>
struct fmt::formatter<itch::order_executed>
{
    constexpr auto
    parse(format_parse_context& ctx)
    {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto
    format(itch::order_executed const& m, FormatContext& ctx)
    {
        return format_to(ctx.out(),
                "order_executed(length={},msg_type={},stock_locate={},tracking_number={},timestamp={},order_reference_number={},executed_shares={},match_number={})",
                be16toh(m.length), m.msg_type, be16toh(m.stock_locate), be16toh(m.tracking_number),
                to_local_time(from_itch_timestamp(m.timestamp)), be64toh(m.order_reference_number),
                be32toh(m.executed_shares), be64toh(m.match_number));
    }
};

template <>
struct fmt::formatter<itch::order_executed_with_price>
{
    constexpr auto
    parse(format_parse_context& ctx)
    {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto
    format(itch::order_executed_with_price const& m, FormatContext& ctx)
    {
        return format_to(ctx.out(),
                "order_executed_with_price(length={},msg_type={},stock_locate={},tracking_number={},timestamp={},order_reference_number={},executed_shares={},match_number={},printable={},execution_price={})",
                be16toh(m.length), m.msg_type, be16toh(m.stock_locate), be16toh(m.tracking_number),
                to_local_time(from_itch_timestamp(m.timestamp)), be64toh(m.order_reference_number),
                be32toh(m.executed_shares), be64toh(m.match_number), m.printable,
                be32toh(m.execution_price));
    }
};

template <>
struct fmt::formatter<itch::order_cancel>
{
    constexpr auto
    parse(format_parse_context& ctx)
    {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto
    format(itch::order_cancel const& m, FormatContext& ctx)
    {
        return format_to(ctx.out(),
                "order_cancel(length={},msg_type={},stock_locate={},tracking_number={},timestamp={},order_reference_number={},cancelled_shares={})",
                be16toh(m.length), m.msg_type, be16toh(m.stock_locate), be16toh(m.tracking_number),
                to_local_time(from_itch_timestamp(m.timestamp)), be64toh(m.order_reference_number),
                be32toh(m.cancelled_shares));
    }
};

template <>
struct fmt::formatter<itch::order_delete>
{
    constexpr auto
    parse(format_parse_context& ctx)
    {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto
    format(itch::order_delete const& m, FormatContext& ctx)
    {
        return format_to(ctx.out(),
                "order_delete(length={},msg_type={},stock_locate={},tracking_number={},timestamp={},order_reference_number={})",
                be16toh(m.length), m.msg_type, be16toh(m.stock_locate), be16toh(m.tracking_number),
                to_local_time(from_itch_timestamp(m.timestamp)), be64toh(m.order_reference_number));
    }
};

template <>
struct fmt::formatter<itch::order_replace>
{
    constexpr auto
    parse(format_parse_context& ctx)
    {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto
    format(itch::order_replace const& m, FormatContext& ctx)
    {
        return format_to(ctx.out(),
                "order_replace(length={},msg_type={},stock_locate={},tracking_number={},timestamp={},original_order_reference_number={},new_order_reference_number={},shares={},price={})",
                be16toh(m.length), m.msg_type, be16toh(m.stock_locate), be16toh(m.tracking_number),
                to_local_time(from_itch_timestamp(m.timestamp)),
                be64toh(m.original_order_reference_number), be64toh(m.new_order_reference_number),
                be32toh(m.shares), be32toh(m.price));
    }
};

template <>
struct fmt::formatter<itch::trade_non_cross>
{
    constexpr auto
    parse(format_parse_context& ctx)
    {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto
    format(itch::trade_non_cross const& m, FormatContext& ctx)
    {
        return format_to(ctx.out(),
                "trade_non_cross(length={},msg_type={},stock_locate={},tracking_number={},timestamp={},order_reference_number={},buy_sell_indicator={},shares={},stock={:.{}},price={},match_number={})",
                be16toh(m.length), m.msg_type, be16toh(m.stock_locate), be16toh(m.tracking_number),
                to_local_time(from_itch_timestamp(m.timestamp)), be64toh(m.order_reference_number),
                m.buy_sell_indicator, be32toh(m.shares), m.stock, sizeof(m.stock), be32toh(m.price),
                be64toh(m.match_number));
    }
};

template <>
struct fmt::formatter<itch::trade_cross>
{
    constexpr auto
    parse(format_parse_context& ctx)
    {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto
    format(itch::trade_cross const& m, FormatContext& ctx)
    {
        return format_to(ctx.out(),
                "trade_cross(length={},msg_type={},stock_locate={},tracking_number={},timestamp={},shares={},stock={:.{}},cross_price={},match_number={},cross_type={})",
                be16toh(m.length), m.msg_type, be16toh(m.stock_locate), be16toh(m.tracking_number),
                to_local_time(from_itch_timestamp(m.timestamp)), be32toh(m.shares), m.stock,
                sizeof(m.stock), be32toh(m.cross_price), be64toh(m.match_number), m.cross_type);
    }
};

template <>
struct fmt::formatter<itch::broken_trade>
{
    constexpr auto
    parse(format_parse_context& ctx)
    {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto
    format(itch::broken_trade const& m, FormatContext& ctx)
    {
        return format_to(ctx.out(),
                "broken_trade(length={},msg_type={},stock_locate={},tracking_number={},timestamp={},match_number={})",
                be16toh(m.length), m.msg_type, be16toh(m.stock_locate), be16toh(m.tracking_number),
                to_local_time(from_itch_timestamp(m.timestamp)), be64toh(m.match_number));
    }
};

template <>
struct fmt::formatter<itch::noii>
{
    constexpr auto
    parse(format_parse_context& ctx)
    {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto
    format(itch::noii const& m, FormatContext& ctx)
    {
        return format_to(ctx.out(),
                "noii(length={},msg_type={},stock_locate={},tracking_number={},timestamp={},paired_shares={},imbalance_shares={},imbalance_direction={},stock={:.{}},far_price={},near_price={},current_reference_price={},cross_type={},price_variation_indicator={})",
                be16toh(m.length), m.msg_type, be16toh(m.stock_locate), be16toh(m.tracking_number),
                to_local_time(from_itch_timestamp(m.timestamp)), be64toh(m.paired_shares),
                be64toh(m.imbalance_shares), m.imbalance_direction, m.stock, sizeof(m.stock),
                be32toh(m.far_price), be32toh(m.near_price), be32toh(m.current_reference_price),
                m.cross_type, m.price_variation_indicator);
    }
};
