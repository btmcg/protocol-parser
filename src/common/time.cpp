#include "time.hpp"


std::string
to_utc_str(std::uint64_t nsecs)
{
    std::time_t const t = nsecs /= tsc::NsecInSec;
    return fmt::format("{:%Y-%m-%d %H:%M:%S}.{:09}", *std::gmtime(&t), nsecs);
}

std::string
to_time_str(std::uint64_t nsecs)
{
    std::uint64_t s = nsecs / tsc::NsecInSec;
    nsecs %= tsc::NsecInSec;
    std::uint64_t m = s / 60;
    s %= 60;
    std::uint64_t h = m / 60;
    m %= 60;
    return fmt::format("{:02}:{:02}:{:02}.{:09}", h, m, s, nsecs);
}
