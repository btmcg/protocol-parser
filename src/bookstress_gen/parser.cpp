#include "parser.hpp"
#include "protocol/custom/bookstress.hpp"


namespace book_stress {


    parser::parser(std::filesystem::path const& output)
    {
        if (output.empty())
            throw std::runtime_error("parser: missing output file");

        output_ = std::fopen(output.c_str(), "wb");
    }

    parser::~parser() noexcept
    {
        std::fclose(output_);
    }


    std::size_t
    parser::parse(std::uint8_t const* buf, std::size_t bytes_to_read) noexcept
    {
        std::size_t bytes_processed = 0;
        std::uint8_t const* end = buf + bytes_to_read;
        while (buf + sizeof(itch::header) < end) {
            auto hdr = reinterpret_cast<itch::header const*>(buf);
            std::uint16_t const msg_len = be16toh(hdr->length) + sizeof(hdr->length);

            // not enough bytes for the full msg, read nothing
            if (buf + msg_len > end)
                break;

            // clang-format off
            // switch ordered by msg count on sample day (2020-01-30)
            switch (hdr->msg_type) {
                case 'A': handle_add_order(reinterpret_cast<itch::add_order const*>(hdr)); break;
                case 'F': handle_add_order_with_mpid(reinterpret_cast<itch::add_order_with_mpid const*>(hdr)); break;
                case 'C': handle_order_executed_with_price(reinterpret_cast<itch::order_executed_with_price const*>(hdr)); break;
                case 'D': handle_order_delete(reinterpret_cast<itch::order_delete const*>(hdr)); break;
                case 'E': handle_order_executed(reinterpret_cast<itch::order_executed const*>(hdr)); break;
                case 'U': handle_order_replace(reinterpret_cast<itch::order_replace const*>(hdr)); break;
                case 'X': handle_order_cancel(reinterpret_cast<itch::order_cancel const*>(hdr)); break;
                default: break;
            }
            // clang-format on

            buf += msg_len;
            bytes_processed += msg_len;
        }
        return bytes_processed;
    }


    void
    parser::handle_add_order(itch::add_order const* m)
    {
        msg msg;
        std::memset(&msg, 0, sizeof(msg));

        msg.type = 'A';
        msg.buy_sell = m->buy_sell_indicator == 'B' ? 1 : 2;
        msg.ts = itch::from_itch_timestamp(m->timestamp);
        msg.order_reference_number = be64toh(m->order_reference_number);
        msg.qty = be32toh(m->shares);
        msg.price = be32toh(m->price);

        std::fwrite(&msg, 1, sizeof(msg), output_);
    }

    void
    parser::handle_add_order_with_mpid(itch::add_order_with_mpid const* m)
    {
        msg msg;
        std::memset(&msg, 0, sizeof(msg));

        msg.type = 'A';
        msg.buy_sell = m->buy_sell_indicator == 'B' ? 1 : 2;
        msg.ts = itch::from_itch_timestamp(m->timestamp);
        msg.order_reference_number = be64toh(m->order_reference_number);
        msg.qty = be32toh(m->shares);
        msg.price = be32toh(m->price);

        std::fwrite(&msg, 1, sizeof(msg), output_);
    }

    void
    parser::handle_order_executed_with_price(itch::order_executed_with_price const* m)
    {
        msg msg;
        std::memset(&msg, 0, sizeof(msg));

        msg.type = 'C';
        msg.ts = itch::from_itch_timestamp(m->timestamp);
        msg.order_reference_number = be64toh(m->order_reference_number);
        msg.qty = be32toh(m->executed_shares);
        msg.price = be32toh(m->execution_price);

        std::fwrite(&msg, 1, sizeof(msg), output_);
    }

    void
    parser::handle_order_delete(itch::order_delete const* m)
    {
        msg msg;
        std::memset(&msg, 0, sizeof(msg));

        msg.type = 'D';
        msg.ts = itch::from_itch_timestamp(m->timestamp);
        msg.order_reference_number = be64toh(m->order_reference_number);

        std::fwrite(&msg, 1, sizeof(msg), output_);
    }

    void
    parser::handle_order_executed(itch::order_executed const* m)
    {
        msg msg;
        std::memset(&msg, 0, sizeof(msg));

        msg.type = 'C';
        msg.ts = itch::from_itch_timestamp(m->timestamp);
        msg.order_reference_number = be64toh(m->order_reference_number);
        msg.qty = be32toh(m->executed_shares);

        std::fwrite(&msg, 1, sizeof(msg), output_);
    }

    void
    parser::handle_order_replace(itch::order_replace const* m)
    {
        msg msg;
        std::memset(&msg, 0, sizeof(msg));

        msg.type = 'U';
        msg.ts = itch::from_itch_timestamp(m->timestamp);
        msg.order_reference_number = be64toh(m->original_order_reference_number);
        msg.qty = be32toh(m->shares);
        msg.price = be32toh(m->price);
        msg.new_order_reference_number = be64toh(m->new_order_reference_number);

        std::fwrite(&msg, 1, sizeof(msg), output_);
    }

    void
    parser::handle_order_cancel(itch::order_cancel const* m)
    {
        msg msg;
        std::memset(&msg, 0, sizeof(msg));

        msg.type = 'X';
        msg.ts = itch::from_itch_timestamp(m->timestamp);
        msg.order_reference_number = be64toh(m->order_reference_number);
        msg.qty = be32toh(m->cancelled_shares);

        std::fwrite(&msg, 1, sizeof(msg), output_);
    }


} // namespace book_stress
