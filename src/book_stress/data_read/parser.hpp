#pragma once

#include "protocol/custom/book_stress.hpp"
#include <filesystem>
#include <cstddef> // std::size_t
#include <cstdint>


namespace book_stress::data_read {

    class parser
    {
    public:
        parser() noexcept = default;
        ~parser() noexcept = default;
        std::size_t parse(std::uint8_t const* buf, std::size_t buf_len) noexcept;

    private:
        void handle_order_add(book_stress::message const*); // A
        void handle_order_delete(book_stress::message const*); // D
        void handle_order_executed(book_stress::message const*); // E
        void handle_order_replace(book_stress::message const*); // U
        void handle_order_cancel(book_stress::message const*); // X
    };

} // namespace book_stress::data_read
