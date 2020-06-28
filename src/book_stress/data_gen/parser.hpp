#pragma once

#include "protocol/itch/itch-fmt.hpp"
#include "protocol/itch/itch.hpp"
#include <filesystem>
#include <cstddef> // std::size_t
#include <cstdint>
#include <cstdio> // std::fclose, std::fopen


namespace book_stress::data_gen {

    class parser
    {
    private:
        std::FILE* output_ = nullptr;

    public:
        explicit parser(std::filesystem::path const& output);
        ~parser() noexcept;
        std::size_t parse(std::uint8_t const* buf, std::size_t bytes_to_read) noexcept;

    private:
        void handle_add_order(itch::add_order const*);
        void handle_add_order_with_mpid(itch::add_order_with_mpid const*);
        void handle_order_executed(itch::order_executed const*);
        void handle_order_executed_with_price(itch::order_executed_with_price const*);
        void handle_order_delete(itch::order_delete const*);
        void handle_order_replace(itch::order_replace const*);
        void handle_order_cancel(itch::order_cancel const*);
    };

} // namespace book_stress::data_gen
