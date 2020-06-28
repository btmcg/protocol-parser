#include "parser.hpp"
#include <fmt/format.h>


namespace book_stress::stressor {


    parser::~parser() noexcept
    {
        fmt::print("adds     {}\n"
                   "deletes  {}\n"
                   "execs    {}\n"
                   "replaces {}\n"
                   "cancels  {}\n",
                   stats_.adds, stats_.deletes, stats_.execs, stats_.replaces, stats_.cancels);
    }

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
        ++stats_.adds;

        itch::order o;
        o.ts = m->ts;
        o.side = static_cast<itch::Side>(m->side);
        o.price = m->price;
        o.qty = m->qty;
        book_.add_order(o);
        order_mapping_[m->oid] = o;
    }

    void
    parser::handle_order_delete(book_stress::message const* m)
    {
        ++stats_.deletes;

        auto o_itr = order_mapping_.find(m->oid);
        if (o_itr == order_mapping_.end()) {
            fmt::print(stderr, "{}: unknown oid {}\n", __builtin_FUNCTION(), m->oid);
            return;
        }
        itch::order& o = o_itr->second;

        book_.delete_order(o);
        o.clear();
    }

    void
    parser::handle_order_executed(book_stress::message const* m)
    {
        ++stats_.execs;

        auto o_itr = order_mapping_.find(m->oid);
        if (o_itr == order_mapping_.end()) {
            fmt::print(stderr, "{}: unknown oid {}\n", __builtin_FUNCTION(), m->oid);
            return;
        }
        itch::order& o = o_itr->second;

        book_.cancel_order(o, m->qty);
    }

    void
    parser::handle_order_replace(book_stress::message const* m)
    {
        ++stats_.replaces;

        auto o_itr = order_mapping_.find(m->oid);
        if (o_itr == order_mapping_.end()) {
            fmt::print(stderr, "{}: unknown oid {}\n", __builtin_FUNCTION(), m->oid);
            return;
        }
        itch::order& old_order = o_itr->second;

        itch::order new_order;
        new_order.side = old_order.side;
        new_order.price = old_order.price;
        new_order.qty = old_order.qty;

        book_.replace_order(old_order, new_order);
        order_mapping_[m->new_oid] = new_order;
    }

    void
    parser::handle_order_cancel(book_stress::message const* m)
    {
        ++stats_.cancels;

        auto o_itr = order_mapping_.find(m->oid);
        if (o_itr == order_mapping_.end()) {
            fmt::print(stderr, "{}: unknown oid {}\n", __builtin_FUNCTION(), m->oid);
            return;
        }
        itch::order& o = o_itr->second;

        book_.cancel_order(o, m->qty);
    }


} // namespace book_stress::stressor
