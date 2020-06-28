#include "parser.hpp"
#include <fmt/format.h>


namespace book_stress::data_read {


    std::size_t
    parser::parse(std::uint8_t const* buf, std::size_t buf_len) noexcept
    {
        std::size_t bytes_processed = 0;
        std::uint8_t const* end = buf + buf_len;
        while (buf + sizeof(book_stress::header) < end) {
            auto hdr = reinterpret_cast<book_stress::header const*>(buf);

            // not enough bytes for the full msg, read nothing
            if (buf + sizeof(book_stress::message) > end)
                break;

            auto msg = reinterpret_cast<book_stress::message const*>(hdr);

            // clang-format off
            switch (hdr->type) {
                case 'A': handle_order_add(msg);        break;
                case 'D': handle_order_delete(msg);     break;
                case 'E': handle_order_executed(msg);   break;
                case 'U': handle_order_replace(msg);    break;
                case 'X': handle_order_cancel(msg);     break;
                default:  std::abort();                 break;
            }
            // clang-format on

            buf += sizeof(book_stress::message);
            bytes_processed += sizeof(book_stress::message);
        }
        return bytes_processed;
    }

    void
    parser::handle_order_add(book_stress::message const* m)
    {
        fmt::print("{}: type={}, ts={}\n", __builtin_FUNCTION(), m->type, m->ts);
    }

    void
    parser::handle_order_delete(book_stress::message const* m)
    {
        fmt::print("{}: type={}, ts={}\n", __builtin_FUNCTION(), m->type, m->ts);
    }

    void
    parser::handle_order_executed(book_stress::message const* m)
    {
        fmt::print("{}: type={}, ts={}\n", __builtin_FUNCTION(), m->type, m->ts);
    }

    void
    parser::handle_order_replace(book_stress::message const* m)
    {
        fmt::print("{}: type={}, ts={}\n", __builtin_FUNCTION(), m->type, m->ts);
    }

    void
    parser::handle_order_cancel(book_stress::message const* m)
    {
        fmt::print("{}: type={}, ts={}\n", __builtin_FUNCTION(), m->type, m->ts);
    }


} // namespace book_stress::data_read
