#pragma once

#include "itch.hpp"
#include <endian.h>
#include <fmt/format.h>

namespace { // unnamed
    std::uint64_t
    from_itch_timestamp(std::uint8_t const (&timestamp)[6])
    {
        std::uint64_t t = 0;
        for (std::size_t i = 0; i < 6; ++i)
            t = (t << 8) + timestamp[i];
        return t;
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
                "system_event(message_type={},stock_locate={},tracking_number={},timestamp={},event_code={})",
                m.message_type, be16toh(m.stock_locate), be16toh(m.tracking_number),
                from_itch_timestamp(m.timestamp), m.event_code);
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
                "stock_directory(message_type={},stock_locate={},tracking_number={},timestamp={},stock={:.{}},market_category={},financial_status_indicator={},round_lot_size={},round_lots_only={},issue_classification={},issue_subtype={:.{}},authenticity={},short_sale_threshold_indicator={},ipo_flag={},luld_reference_price_tier={},etp_flag={},etp_leverage_factor={},inverse_indicator={})",
                m.message_type, be16toh(m.stock_locate), be16toh(m.tracking_number),
                from_itch_timestamp(m.timestamp), m.stock, sizeof(m.stock), m.market_category,
                m.financial_status_indicator, be32toh(m.round_lot_size), m.round_lots_only,
                m.issue_classification, m.issue_subtype, sizeof(m.issue_subtype), m.authenticity,
                m.short_sale_threshold_indicator, m.ipo_flag, m.luld_reference_price_tier,
                m.etp_flag, be32toh(m.etp_leverage_factor), m.inverse_indicator);
    }
};
