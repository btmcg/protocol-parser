#include "protocol/itch-fmt.hpp"
#include <catch2/catch.hpp>
#include <fmt/chrono.h>
#include <fmt/format.h>
#include <ctime>
#include <string>


TEST_CASE("get_midnight", "[itch-fmt]")
{
    std::time_t now = std::time(nullptr);
    tm midnight_tm;
    ::localtime_r(&now, &midnight_tm);
    midnight_tm.tm_sec = midnight_tm.tm_min = midnight_tm.tm_hour = 0;
    std::time_t const midnight = std::mktime(&midnight_tm);
    std::uint64_t const midnight_nsec = midnight * 1000 * 1000 * 1000;

    REQUIRE(get_midnight_nsecs() == midnight_nsec);
    REQUIRE(MidnightNsec == midnight_nsec);
}

TEST_CASE("to_local_time", "[itch-fmt]")
{
    std::uint64_t const nsecs_since_midnight = 72000087028229;
    REQUIRE(to_local_time(nsecs_since_midnight) == "20:00:00.087028229");
    REQUIRE(to_local_time(nsecs_since_midnight - 1) == "20:00:00.087028228");
    REQUIRE(to_local_time(nsecs_since_midnight + 1) == "20:00:00.087028230");
}
