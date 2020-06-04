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
    static constexpr std::uint64_t secs_in_min = 60;
    static constexpr std::uint64_t mins_in_hour = 60;

    std::uint64_t s = nsecs / tsc::NsecInSec;
    nsecs %= tsc::NsecInSec;
    std::uint64_t m = s / secs_in_min;
    s %= secs_in_min;
    std::uint64_t h = m / mins_in_hour;
    m %= mins_in_hour;
    return fmt::format("{:02}:{:02}:{:02}.{:09}", h, m, s, nsecs);
}
