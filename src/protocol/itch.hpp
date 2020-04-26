#pragma once

#include <cstdint>

#define PACKED __attribute__((packed))


namespace itch {
    inline namespace v5_0 {

        /// synthetic msg; not strictly part of itch protocol, but
        /// contains fields that are common to all messages.
        struct header
        {
            std::uint16_t length; ///< this field does not add to the message length
            char message_type;
            std::uint16_t stock_locate;
            std::uint16_t tracking_number;
            std::uint8_t timestamp[6];
        } PACKED;
        static_assert(sizeof(header) == 13);

        /// message_type='S'
        struct system_event : public header
        {
            char event_code;
        } PACKED;
        static_assert(sizeof(system_event) - sizeof(header::length) == 12);

        /// message_type='R'
        struct stock_directory : public header
        {
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
        static_assert(sizeof(stock_directory) - sizeof(header::length) == 39);

        /// message_type='H'
        struct stock_trading_action : public header
        {
            char stock[8];
            char trading_state;
            char reserved;
            char reason[4];
        } PACKED;
        static_assert(sizeof(stock_trading_action) - sizeof(header::length) == 25);

        /// message_type='Y'
        struct reg_sho_restriction : public header
        {
            char stock[8];
            char reg_sho_action;
        } PACKED;
        static_assert(sizeof(reg_sho_restriction) - sizeof(header::length) == 20);

        /// message_type='L'
        struct market_participant_position : public header
        {
            char mpid[4];
            char stock[8];
            char primary_market_maker;
            char market_maker_mode;
            char market_participant_state;
        } PACKED;
        static_assert(sizeof(market_participant_position) - sizeof(header::length) == 26);

        /// message_type='V'
        struct mwcb_decline_level : public header
        {
            std::uint64_t level1;
            std::uint64_t level2;
            std::uint64_t level3;
        } PACKED;
        static_assert(sizeof(mwcb_decline_level) - sizeof(header::length) == 35);

        /// message_type='W'
        struct mwcb_status : public header
        {
            char breached_level;
        } PACKED;
        static_assert(sizeof(mwcb_status) - sizeof(header::length) == 12);

        /// message_type='K'
        struct ipo_quoting_period_update : public header
        {
            char stock[8];
            std::uint32_t ipo_quotation_release_time;
            char ipo_quotation_release_qualifier;
            std::uint32_t ipo_price;
        } PACKED;
        static_assert(sizeof(ipo_quoting_period_update) - sizeof(header::length) == 28);

        /// message_type='J'
        struct luld_auction_collar : public header
        {
            char stock[8];
            std::uint32_t auction_collar_reference_price;
            std::uint32_t upper_auction_collar_price;
            std::uint32_t lower_auction_collar_price;
            std::uint32_t auction_collar_extension;
        } PACKED;
        static_assert(sizeof(luld_auction_collar) - sizeof(header::length) == 35);

        /// message_type='h'
        struct operational_halt : public header
        {
            char stock[8];
            char market_code;
            char operational_halt_action;
        } PACKED;
        static_assert(sizeof(operational_halt) - sizeof(header::length) == 21);

        /// message_type='A'
        struct add_order : public header
        {
            std::uint64_t order_reference_number;
            char buy_sell_indicator;
            std::uint32_t shares;
            char stock[8];
            std::uint32_t price;
        } PACKED;
        static_assert(sizeof(add_order) - sizeof(header::length) == 36);

        /// message_type='F'
        struct add_order_with_mpid : public header
        {
            std::uint64_t order_reference_number;
            char buy_sell_indicator;
            std::uint32_t shares;
            char stock[8];
            std::uint32_t price;
            char attribution[4];
        } PACKED;
        static_assert(sizeof(add_order_with_mpid) - sizeof(header::length) == 40);

        /// message_type='E'
        struct order_executed : public header
        {
            std::uint64_t order_reference_number;
            std::uint32_t executed_shares;
            std::uint64_t match_number;
        } PACKED;
        static_assert(sizeof(order_executed) - sizeof(header::length) == 31);

        /// message_type='C'
        struct order_executed_with_price : public header
        {
            std::uint64_t order_reference_number;
            std::uint32_t executed_shares;
            std::uint64_t match_number;
            char printable;
            std::uint32_t execution_price;
        } PACKED;
        static_assert(sizeof(order_executed_with_price) - sizeof(header::length) == 36);

        /// message_type='X'
        struct order_cancel : public header
        {
            std::uint64_t order_reference_number;
            std::uint32_t cancelled_shares;
        } PACKED;
        static_assert(sizeof(order_cancel) - sizeof(header::length) == 23);

        /// message_type='D'
        struct order_delete : public header
        {
            std::uint64_t order_reference_number;
        } PACKED;
        static_assert(sizeof(order_delete) - sizeof(header::length) == 19);

        /// message_type='U'
        struct order_replace : public header
        {
            std::uint64_t original_order_reference_number;
            std::uint64_t new_order_reference_number;
            std::uint32_t shares;
            std::uint32_t price;
        } PACKED;
        static_assert(sizeof(order_replace) - sizeof(header::length) == 35);

        /// message_type='P'
        struct trade_non_cross : public header
        {
            std::uint64_t order_reference_number;
            char buy_sell_indicator;
            std::uint32_t shares;
            char stock[8];
            std::uint32_t price;
            std::uint64_t match_number;
        } PACKED;
        static_assert(sizeof(trade_non_cross) - sizeof(header::length) == 44);

        /// message_type='Q'
        struct trade_cross : public header
        {
            std::uint64_t shares;
            char stock[8];
            std::uint32_t cross_price;
            std::uint64_t match_number;
            char cross_type;
        } PACKED;
        static_assert(sizeof(trade_cross) - sizeof(header::length) == 40);

        /// message_type='B'
        struct broken_trade : public header
        {
            std::uint64_t match_number;
        } PACKED;
        static_assert(sizeof(broken_trade) - sizeof(header::length) == 19);

        /// message_type='I'
        struct noii : public header
        {
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
        static_assert(sizeof(noii) - sizeof(header::length) == 50);

    } // namespace v5_0
} // namespace itch

#undef PACKED
