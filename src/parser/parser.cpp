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
            fmt::print(stderr, "ERROR: open ({}): [{}]\n", filepath.c_str(), std::strerror(errno));
            return {nullptr, 0};
        }

        off_t const file_size = ::lseek(fd, 0, SEEK_END);
        if (file_size == -1) {
            fmt::print(stderr, "ERROR: lseek [{}]\n", std::strerror(errno));
            ::close(fd);
            return {nullptr, 0};
        }

        void* f_ptr = ::mmap(nullptr, file_size, PROT_READ, MAP_PRIVATE, fd, 0);
        if (f_ptr == MAP_FAILED) {
            fmt::print(stderr, "ERROR: mmap [{}]\n", std::strerror(errno));
            ::close(fd);
            return {nullptr, 0};
        }

        if (::close(fd) == -1) {
            // not a critical failure
            fmt::print(stderr, "ERROR: close [{}]\n", std::strerror(errno));
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
        fmt::print(stderr, "ERROR: munmap [{}]\n", std::strerror(errno));
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
        fmt::print(stderr, "ERROR: inflateInit returned error {}\n", rv);
        return false;
    }

    std::uint8_t buf[BufferSize];

    std::uint8_t* ptr = reinterpret_cast<decltype(ptr)>(f_ptr_);
    // std::uint8_t const* eof = ptr + file_size_;
    // std::uint64_t bytes_left = eof - ptr;
    // while (bytes_left > 0) {}
    zstrm.avail_in = file_size_;
    zstrm.next_in = ptr;

    int ret = 0;
    do {
        zstrm.avail_out = BufferSize;
        zstrm.next_out = buf;

        ret = ::inflate(&zstrm, Z_NO_FLUSH);
        assert(ret != Z_OK);
        switch (ret) {
            case Z_NEED_DICT:
                fmt::print(stderr, "Z_NEED_DICT\n");
                return false;
            case Z_DATA_ERROR:
                fmt::print(stderr, "Z_DATA_ERROR\n");
                return false;
            case Z_MEM_ERROR:
                fmt::print(stderr, "Z_MEM_ERROR\n");
                (void)inflateEnd(&zstrm);
                return false;
        }

        auto nbytes = parse_itch(buf);
        nbytes = parse_itch(buf + nbytes);

        fmt::print("avail_out={}, total_out={}\n", zstrm.avail_out, zstrm.total_out);
    } while (ret != Z_STREAM_END);
    fmt::print("Z_STREAM_END\n");

    return true;
}

std::uint16_t
parser::parse_itch(std::uint8_t const* buf) noexcept
{
    using namespace itch;

    std::uint16_t const len = be16toh(*reinterpret_cast<std::uint16_t const*>(buf)) + sizeof(len);
    fmt::print("len={}\n", len);
    buf += sizeof(len);

    // clang-format off
    switch (*buf) {
        case 'S': fmt::print("{}\n", *reinterpret_cast<system_event const*>(buf)); break;
        case 'R': fmt::print("{}\n", *reinterpret_cast<stock_directory const*>(buf)); break;
        case 'H': fmt::print("{}\n", *reinterpret_cast<stock_trading_action const*>(buf)); break;
        case 'Y': fmt::print("{}\n", *reinterpret_cast<reg_sho_restriction const*>(buf)); break;
        case 'L': fmt::print("{}\n", *reinterpret_cast<market_participant_position const*>(buf)); break;
        case 'V': fmt::print("{}\n", *reinterpret_cast<mwcb_decline_level const*>(buf)); break;
        case 'W': fmt::print("{}\n", *reinterpret_cast<mwcb_status const*>(buf)); break;
        case 'K': fmt::print("{}\n", *reinterpret_cast<ipo_quoting_period_update const*>(buf)); break;
        case 'J': fmt::print("{}\n", *reinterpret_cast<luld_auction_collar const*>(buf)); break;
        case 'h': fmt::print("{}\n", *reinterpret_cast<operational_halt const*>(buf)); break;
        case 'A': fmt::print("{}\n", *reinterpret_cast<add_order const*>(buf)); break;
        case 'F': fmt::print("{}\n", *reinterpret_cast<add_order_with_mpid const*>(buf)); break;
        case 'E': fmt::print("{}\n", *reinterpret_cast<order_executed const*>(buf)); break;
        case 'C': fmt::print("{}\n", *reinterpret_cast<order_executed_with_price const*>(buf)); break;
        case 'X': fmt::print("{}\n", *reinterpret_cast<order_cancel const*>(buf)); break;
        case 'D': fmt::print("{}\n", *reinterpret_cast<order_delete const*>(buf)); break;
        case 'U': fmt::print("{}\n", *reinterpret_cast<order_replace const*>(buf)); break;
        case 'P': fmt::print("{}\n", *reinterpret_cast<trade_non_cross const*>(buf)); break;
        case 'Q': fmt::print("{}\n", *reinterpret_cast<trade_cross const*>(buf)); break;
        case 'B': fmt::print("{}\n", *reinterpret_cast<broken_trade const*>(buf)); break;
        case 'I': fmt::print("{}\n", *reinterpret_cast<noii const*>(buf)); break;

        default: fmt::print("unknown type=[{:c}]\n", reinterpret_cast<char const*>(buf)); break;
    }
    // clang-format on

    return len;
}

// for (int i = 0; i < 10; ++i) {
//     if (std::isalpha(ptr[i]))
//         fmt::print("[{:c}] ", (char)ptr[i]);
//     else
//         fmt::print("[{:x}] ", ptr[i]);
// }
