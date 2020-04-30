#include "common/time.hpp"
#include <catch.hpp>
#include <cstdint>
#include <ctime>


TEST_CASE("init", "[tsc]")
{
    tsc::init();

    REQUIRE(tsc::gettime_nsec() != 0);
    timespec ts = tsc::gettime_ts();
    REQUIRE(ts.tv_sec != 0);
    REQUIRE(ts.tv_nsec != 0);
    REQUIRE(tsc::get_cycles_per_nsec() != Approx(0.0));
    REQUIRE(tsc::get_nsecs() != 0);
}

TEST_CASE("free functions", "[time]")
{
    SECTION("ts_diff")
    {
        timespec t1 = {10, 1000};
        timespec t2 = {10, 1500};
        timespec diff = {0, 0};

        // invalid diff
        diff = ts_diff(t2, t1);
        REQUIRE(diff.tv_sec == 0);
        REQUIRE(diff.tv_nsec == 0);

        // nsec diff
        diff = ts_diff(t1, t2);
        REQUIRE(diff.tv_sec == 0);
        REQUIRE(diff.tv_nsec == 500);

        // sec diff
        t2.tv_sec = 12;
        diff = ts_diff(t1, t2);
        REQUIRE(diff.tv_sec == 2);
        REQUIRE(diff.tv_nsec == 500);
    }

    SECTION("to_timespec")
    {
        timespec ts = {0, 0};

        // nsecs
        ts = to_timespec(1000);
        REQUIRE(ts.tv_sec == 0);
        REQUIRE(ts.tv_nsec == 1000);

        // secs
        ts = to_timespec(1234 * 1e8);
        REQUIRE(ts.tv_sec == 123);
        REQUIRE(ts.tv_nsec == 400'000'000);

        // zero
        ts = to_timespec(0);
        REQUIRE(ts.tv_sec == 0);
        REQUIRE(ts.tv_nsec == 0);
    }

    SECTION("to_nsecs")
    {
        timespec ts = {0, 0};

        // nsecs
        ts.tv_sec = 0;
        ts.tv_nsec = 1000;
        REQUIRE(to_nsecs(ts) == 1000);

        // secs
        ts.tv_sec = 123;
        ts.tv_nsec = 400'000'000;
        REQUIRE(to_nsecs(ts) == 123'400'000'000);

        // zero
        ts.tv_sec = 0;
        ts.tv_nsec = 0;
        REQUIRE(to_nsecs(ts) == 0);
    }
}
