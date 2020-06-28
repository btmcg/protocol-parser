#pragma once

#include "itch/basic_book.hpp"
#include "itch/core.hpp"
#include "protocol/custom/book_stress.hpp"
#include <cstddef> // std::size_t
#include <cstdint>
#include <unordered_map>


namespace book_stress::stressor {


    struct stats
    {
        std::uint32_t adds = 0;
        std::uint32_t deletes = 0;
        std::uint32_t execs = 0;
        std::uint32_t replaces = 0;
        std::uint32_t cancels = 0;
    };

    class parser
    {
    private:
        stats stats_;
        std::unordered_map<itch::oid_t, itch::order> order_mapping_;
        itch::basic_book book_;

    public:
        parser() noexcept = default;
        ~parser() noexcept;
        std::size_t parse(std::uint8_t const* buf, std::size_t buf_len) noexcept;

    private:
        void handle_order_add(book_stress::message const*); // A
        void handle_order_delete(book_stress::message const*); // D
        void handle_order_executed(book_stress::message const*); // E
        void handle_order_replace(book_stress::message const*); // U
        void handle_order_cancel(book_stress::message const*); // X
    };

} // namespace book_stress::stressor
