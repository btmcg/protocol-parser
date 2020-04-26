#include "parser.hpp"
#include "protocol/itch-fmt.hpp"
#include "protocol/itch.hpp"
#include <endian.h>
#include <fcntl.h> // ::open
#include <fmt/format.h>
#include <sys/mman.h> // ::mmap, ::munmap
#include <unistd.h> // ::lseek
#include <zlib.h>
#include <cassert>
#include <cstdint>
#include <cstring> // std::strerror
#include <iostream>
#include <tuple>


namespace { // unnamed

    std::tuple<void*, off_t>
    mmap_file(std::filesystem::path const& filepath) noexcept
    {
        int const fd = ::open(filepath.c_str(), O_RDONLY);
        if (fd == -1) {
            fmt::print("ERROR: open ({}): [{}]\n", filepath.c_str(), std::strerror(errno));
            return {nullptr, 0};
        }

        off_t const file_size = ::lseek(fd, 0, SEEK_END);
        if (file_size == -1) {
            fmt::print("ERROR: lseek [{}]\n", std::strerror(errno));
            ::close(fd);
            return {nullptr, 0};
        }

        void* f_ptr = ::mmap(nullptr, file_size, PROT_READ, MAP_PRIVATE, fd, 0);
        if (f_ptr == MAP_FAILED) {
            fmt::print("ERROR: mmap [{}]\n", std::strerror(errno));
            ::close(fd);
            return {nullptr, 0};
        }

        if (::close(fd) == -1) {
            // not a critical failure
            fmt::print("ERROR: close [{}]\n", std::strerror(errno));
        }

        return std::make_tuple(f_ptr, file_size);
    }

} // namespace

parser::parser(std::filesystem::path const& input_file)
        : input_file_(input_file)
        , f_ptr_(nullptr)
        , file_size_(0)
{
    std::tie(f_ptr_, file_size_) = mmap_file(input_file_);
    if (f_ptr_ == nullptr)
        throw std::runtime_error(fmt::format("failed to mmap file: {}", input_file_.c_str()));
}

parser::~parser() noexcept
{
    if (::munmap(f_ptr_, file_size_) == -1)
        fmt::print("ERROR: munmap [{}]\n", std::strerror(errno));
}

bool
parser::parse() noexcept
{
    fmt::print("file_size_={}\n", file_size_);

    ::z_stream zstrm;
    zstrm.zalloc = nullptr;
    zstrm.zfree = nullptr;
    zstrm.opaque = nullptr;
    zstrm.avail_in = 0;
    zstrm.next_in = nullptr;

    if (int const rv = ::inflateInit2(&zstrm, 32); rv != Z_OK) {
        fmt::print("ERROR: inflateInit returned error {}\n", rv);
        return false;
    }

    byte_buffer<BufferSize> buf;
    std::size_t total_bytes_inflated = 0;

    zstrm.avail_in = file_size_;
    zstrm.next_in = reinterpret_cast<decltype(zstrm.next_in)>(f_ptr_);

    int ret = 0;
    do {
        zstrm.avail_out = buf.bytes_left();
        zstrm.next_out = buf.write_ptr();

        ret = ::inflate(&zstrm, Z_NO_FLUSH);
        switch (ret) {
            case Z_STREAM_ERROR:
                fmt::print("Z_NEED_DICT\n");
                return false;
            case Z_NEED_DICT:
                fmt::print("Z_NEED_DICT\n");
                return false;
            case Z_DATA_ERROR:
                fmt::print("Z_DATA_ERROR\n");
                return false;
            case Z_MEM_ERROR:
                fmt::print("Z_MEM_ERROR\n");
                (void)inflateEnd(&zstrm);
                return false;
        }
        buf.bytes_written(zstrm.total_out - total_bytes_inflated);
        total_bytes_inflated = zstrm.total_out;

        parse_itch(buf);
        buf.shift();

    } while (ret != Z_STREAM_END);
    fmt::print("parse(): Z_STREAM_END, total_bytes_inflated={}\n", total_bytes_inflated);

    return true;
}

void
parser::parse_itch(byte_buffer<BufferSize>& buf) noexcept
{
    using namespace itch;

    while (buf.bytes_unread() > sizeof(header)) {
        header const* hdr = reinterpret_cast<decltype(hdr)>(buf.read_ptr());
        std::uint16_t const msg_len = be16toh(hdr->length) + sizeof(hdr->length);

        // not enough bytes for the full msg, read nothing
        if (buf.bytes_unread() < msg_len)
            break;

        fmt::print(stderr, "parse_itch(): msg_type={}, len={}\n", hdr->message_type, msg_len);

        // clang-format off
        switch (hdr->message_type) {
            case 'S': fmt::print(stderr, "{}\n", *reinterpret_cast<system_event const*>(hdr)); break;
            case 'R': fmt::print(stderr, "{}\n", *reinterpret_cast<stock_directory const*>(hdr)); break;
            case 'H': fmt::print(stderr, "{}\n", *reinterpret_cast<stock_trading_action const*>(hdr)); break;
            case 'Y': fmt::print(stderr, "{}\n", *reinterpret_cast<reg_sho_restriction const*>(hdr)); break;
            case 'L': fmt::print(stderr, "{}\n", *reinterpret_cast<market_participant_position const*>(hdr)); break;
            case 'V': fmt::print(stderr, "{}\n", *reinterpret_cast<mwcb_decline_level const*>(hdr)); break;
            case 'W': fmt::print(stderr, "{}\n", *reinterpret_cast<mwcb_status const*>(hdr)); break;
            case 'K': fmt::print(stderr, "{}\n", *reinterpret_cast<ipo_quoting_period_update const*>(hdr)); break;
            case 'J': fmt::print(stderr, "{}\n", *reinterpret_cast<luld_auction_collar const*>(hdr)); break;
            case 'h': fmt::print(stderr, "{}\n", *reinterpret_cast<operational_halt const*>(hdr)); break;
            case 'A': fmt::print(stderr, "{}\n", *reinterpret_cast<add_order const*>(hdr)); break;
            case 'F': fmt::print(stderr, "{}\n", *reinterpret_cast<add_order_with_mpid const*>(hdr)); break;
            case 'E': fmt::print(stderr, "{}\n", *reinterpret_cast<order_executed const*>(hdr)); break;
            case 'C': fmt::print(stderr, "{}\n", *reinterpret_cast<order_executed_with_price const*>(hdr)); break;
            case 'X': fmt::print(stderr, "{}\n", *reinterpret_cast<order_cancel const*>(hdr)); break;
            case 'D': fmt::print(stderr, "{}\n", *reinterpret_cast<order_delete const*>(hdr)); break;
            case 'U': fmt::print(stderr, "{}\n", *reinterpret_cast<order_replace const*>(hdr)); break;
            case 'P': fmt::print(stderr, "{}\n", *reinterpret_cast<trade_non_cross const*>(hdr)); break;
            case 'Q': fmt::print(stderr, "{}\n", *reinterpret_cast<trade_cross const*>(hdr)); break;
            case 'B': fmt::print(stderr, "{}\n", *reinterpret_cast<broken_trade const*>(hdr)); break;
            case 'I': fmt::print(stderr, "{}\n", *reinterpret_cast<noii const*>(hdr)); break;

            default: fmt::print("parse_itch(): unknown type=[{:c}]\n", hdr->message_type); std::exit(1); break;
        }
        // clang-format on

        stats_.byte_count += msg_len;
        ++stats_.msg_count;
        buf.bytes_read(msg_len);
    }
}

void
parser::print_stats() const noexcept
{
    fmt::print("bytes_processed: {}\nmsgs_processed:  {}\n", stats_.byte_count, stats_.msg_count);
}

// for (int i = 0; i < 10; ++i) {
//     if (std::isalpha(ptr[i]))
//         fmt::print("[{:c}] ", (char)ptr[i]);
//     else
//         fmt::print("[{:x}] ", ptr[i]);
// }