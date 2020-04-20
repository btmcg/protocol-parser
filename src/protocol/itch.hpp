#pragma once

#include <cstdint>

#define PACKED __attribute__((packed))


namespace itch {
    inline namespace v5_0 {

        struct system_event
        {
            char message_type;
            std::uint16_t stock_locate;
            std::uint16_t tracking_number;
            std::uint8_t timestamp[6];
            char event_code;
        } PACKED;
        static_assert(sizeof(system_event) == 12);

        struct stock_directory
        {
            char message_type;
            std::uint16_t stock_locate;
            std::uint16_t tracking_number;
            std::uint8_t timestamp[6];
            char stock[8];
            char market_category;
            char financial_status_indicator;
            std::uint32_t round_lot_size;
            char round_lots_only;
            char issue_classification;
            char issue_subtype[2];
            char authenticity;
            char short_sale_threshold_indicator;
            char ipo_flag;
            char luld_reference_price_tier;
            char etp_flag;
            std::uint32_t etp_leverage_factor;
            char inverse_indicator;
        } PACKED;
        static_assert(sizeof(stock_directory) == 39);

        struct stock_trading_action
        {
            char message_type;
            std::uint16_t stock_locate;
            std::uint16_t tracking_number;
            std::uint8_t timestamp[6];
            char stock[8];
            char trading_state;
            char reserved;
            char reason[4];
        } PACKED;
        static_assert(sizeof(stock_trading_action) == 25);

        struct reg_sho_restriction
        {
            char message_type;
            std::uint16_t stock_locate;
            std::uint16_t tracking_number;
            std::uint8_t timestamp[6];
            char stock[8];
            char reg_sho_action;
        } PACKED;
        static_assert(sizeof(reg_sho_restriction) == 20);

        struct market_participant_position
        {
            char message_type;
            std::uint16_t stock_locate;
            std::uint16_t tracking_number;
            std::uint8_t timestamp[6];
            char mpid[4];
            char stock[8];
            char primary_market_maker;
            char market_maker_mode;
            char market_participant_state;
        } PACKED;
        static_assert(sizeof(market_participant_position) == 26);

        struct mwcb_decline_level
        {
            char message_type;
            std::uint16_t stock_locate;
            std::uint16_t tracking_number;
            std::uint8_t timestamp[6];
            std::uint64_t level1;
            std::uint64_t level2;
            std::uint64_t level3;
        } PACKED;
        static_assert(sizeof(mwcb_decline_level) == 35);

        struct mwcb_status
        {
            char message_type;
            std::uint16_t stock_locate;
            std::uint16_t tracking_number;
            std::uint8_t timestamp[6];
            char breached_level;
        } PACKED;
        static_assert(sizeof(mwcb_status) == 12);

        struct ipo_quoting_period_update
        {
            char message_type;
            std::uint16_t stock_locate;
            std::uint16_t tracking_number;
            std::uint8_t timestamp[6];
            char stock[8];
            std::uint32_t ipo_quotation_release_time;
            char ipo_quotation_release_qualifier;
            std::uint32_t ipo_price;
        } PACKED;
        static_assert(sizeof(ipo_quoting_period_update) == 28);

        struct luld_auction_collar
        {
            char message_type;
            std::uint16_t stock_locate;
            std::uint16_t tracking_number;
            std::uint8_t timestamp[6];
            char stock[8];
            std::uint32_t auction_collar_reference_price;
            std::uint32_t upper_auction_collar_price;
            std::uint32_t lower_auction_collar_price;
            std::uint32_t auction_collar_extension;
        } PACKED;
        static_assert(sizeof(luld_auction_collar) == 35);

        struct operational_halt
        {
            char message_type;
            std::uint16_t stock_locate;
            std::uint16_t tracking_number;
            std::uint8_t timestamp[6];
            char stock[8];
            char market_code;
            char operational_halt_action;
        } PACKED;
        static_assert(sizeof(operational_halt) == 21);

        struct add_order
        {
            char message_type;
            std::uint16_t stock_locate;
            std::uint16_t tracking_number;
            std::uint8_t timestamp[6];
            std::uint64_t order_reference_number;
            char buy_sell_indicator;
            std::uint32_t shares;
            char stock[8];
            std::uint32_t price;
        } PACKED;
        static_assert(sizeof(add_order) == 36);

        struct add_order_with_mpid
        {
            char message_type;
            std::uint16_t stock_locate;
            std::uint16_t tracking_number;
            std::uint8_t timestamp[6];
            std::uint64_t order_reference_number;
            char buy_sell_indicator;
            std::uint32_t shares;
            char stock[8];
            std::uint32_t price;
            char attribution[4];
        } PACKED;
        static_assert(sizeof(add_order_with_mpid) == 40);

        struct order_executed
        {
            char message_type;
            std::uint16_t stock_locate;
            std::uint16_t tracking_number;
            std::uint8_t timestamp[6];
            std::uint64_t order_reference_number;
            std::uint32_t executed_shares;
            std::uint64_t match_number;
        } PACKED;
        static_assert(sizeof(order_executed) == 31);

        struct order_executed_with_price
        {
            char message_type;
            std::uint16_t stock_locate;
            std::uint16_t tracking_number;
            std::uint8_t timestamp[6];
            std::uint64_t order_reference_number;
            std::uint32_t executed_shares;
            std::uint64_t match_number;
            char printable;
            std::uint32_t execution_price;
        } PACKED;
        static_assert(sizeof(order_executed_with_price) == 36);

        struct order_cancel
        {
            char message_type;
            std::uint16_t stock_locate;
            std::uint16_t tracking_number;
            std::uint8_t timestamp[6];
            std::uint64_t order_reference_number;
            std::uint32_t cancelled_shares;
        } PACKED;
        static_assert(sizeof(order_cancel) == 23);

        struct order_delete
        {
            char message_type;
            std::uint16_t stock_locate;
            std::uint16_t tracking_number;
            std::uint8_t timestamp[6];
            std::uint64_t order_reference_number;
        } PACKED;
        static_assert(sizeof(order_delete) == 19);

        struct order_replace
        {
            char message_type;
            std::uint16_t stock_locate;
            std::uint16_t tracking_number;
            std::uint8_t timestamp[6];
            std::uint64_t original_order_reference_number;
            std::uint64_t new_order_reference_number;
            std::uint32_t shares;
            std::uint32_t price;
        } PACKED;
        static_assert(sizeof(order_replace) == 35);

        struct trade_non_cross
        {
            char message_type;
            std::uint16_t stock_locate;
            std::uint16_t tracking_number;
            std::uint8_t timestamp[6];
            std::uint64_t order_reference_number;
            char buy_sell_indicator;
            std::uint32_t shares;
            char stock[8];
            std::uint32_t price;
            std::uint64_t match_number;
        } PACKED;
        static_assert(sizeof(trade_non_cross) == 44);

        struct trade_cross
        {
            char message_type;
            std::uint16_t stock_locate;
            std::uint16_t tracking_number;
            std::uint8_t timestamp[6];
            std::uint64_t shares;
            char stock[8];
            std::uint32_t cross_price;
            std::uint64_t match_number;
            char cross_type;
        } PACKED;
        static_assert(sizeof(trade_cross) == 40);

        struct broken_trade
        {
            char message_type;
            std::uint16_t stock_locate;
            std::uint16_t tracking_number;
            std::uint8_t timestamp[6];
            std::uint64_t match_number;
        } PACKED;
        static_assert(sizeof(broken_trade) == 19);

        struct noii
        {
            char message_type;
            std::uint16_t stock_locate;
            std::uint16_t tracking_number;
            std::uint8_t timestamp[6];
            std::uint64_t paired_shares;
            std::uint64_t imbalance_shares;
            char imbalance_direction;
            char stock[8];
            std::uint32_t far_price;
            std::uint32_t near_price;
            std::uint32_t current_reference_price;
            char cross_type;
            char price_variation_indicator;
        } PACKED;
        static_assert(sizeof(noii) == 50);

    } // namespace v5_0
} // namespace itch

#undef PACKED
