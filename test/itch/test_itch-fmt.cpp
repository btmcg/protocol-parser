#include "protocol/itch-fmt.hpp"
#include <catch2/catch.hpp>
#include <cstdint>
#include <ctime>
#include <string>


TEST_CASE("get_midnight", "[itch-fmt]")
{
    using namespace itch;
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
    using namespace itch;
    std::uint64_t const nsecs_since_midnight = 72000087028229;
    REQUIRE(to_local_time(nsecs_since_midnight) == "20:00:00.087028229");
    REQUIRE(to_local_time(nsecs_since_midnight - 1) == "20:00:00.087028228");
    REQUIRE(to_local_time(nsecs_since_midnight + 1) == "20:00:00.087028230");

    REQUIRE(to_local_time(10953404452051) == "03:02:33.404452051");
    REQUIRE(to_local_time(14400000245942) == "04:00:00.000245942");
    REQUIRE(to_local_time(34200000073183) == "09:30:00.000073183");
    REQUIRE(to_local_time(57600000078520) == "16:00:00.000078520");
    REQUIRE(to_local_time(72000000021649) == "20:00:00.000021649");
    REQUIRE(to_local_time(72300000039817) == "20:05:00.000039817");

    // system_event(length=12,message_type=S,stock_locate=0,tracking_number=0,timestamp=10953404452051,event_code=O)
    // system_event(length=12,message_type=S,stock_locate=0,tracking_number=0,timestamp=14400000245942,event_code=S)
    // system_event(length=12,message_type=S,stock_locate=0,tracking_number=0,timestamp=34200000073183,event_code=Q)
    // system_event(length=12,message_type=S,stock_locate=0,tracking_number=0,timestamp=57600000078520,event_code=M)
    // system_event(length=12,message_type=S,stock_locate=0,tracking_number=0,timestamp=72000000021649,event_code=E)
    // system_event(length=12,message_type=S,stock_locate=0,tracking_number=0,timestamp=72300000039817,event_code=C)

    // system_event(length=12,message_type=S,stock_locate=0,tracking_number=0,timestamp=03:02:33.404452051,event_code=O)
    // system_event(length=12,message_type=S,stock_locate=0,tracking_number=0,timestamp=04:00:00.000245942,event_code=S)
    // system_event(length=12,message_type=S,stock_locate=0,tracking_number=0,timestamp=09:30:00.000073183,event_code=Q)
    // system_event(length=12,message_type=S,stock_locate=0,tracking_number=0,timestamp=16:00:00.000078520,event_code=M)
    // system_event(length=12,message_type=S,stock_locate=0,tracking_number=0,timestamp=20:00:00.000021649,event_code=E)
    // system_event(length=12,message_type=S,stock_locate=0,tracking_number=0,timestamp=20:05:00.000039817,event_code=C)
}
