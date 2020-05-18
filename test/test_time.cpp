#include "util/time.hpp"
#include <catch2/catch.hpp>
#include <fmt/format.h>
#include <cstdint>
#include <ctime>
#include <iostream>
#include <string>


TEST_CASE("init", "[tsc]")
{
    tsc::init();

    SECTION("gettime_nsec", "[tsc]")
    {
        std::time_t const control = std::time(nullptr);
        std::time_t const variable = tsc::gettime_nsec() / tsc::NsecInSec; // convert to secs

        std::tm* tm_c = std::gmtime(&control);
        std::tm* tm_v = std::gmtime(&variable);

        char buf_c[32];
        char buf_v[32];

        std::size_t const sz_c = std::strftime(buf_c, sizeof(buf_c), "%Y-%m-%d %H:%M:%S", tm_c);
        std::size_t const sz_v = std::strftime(buf_v, sizeof(buf_v), "%Y-%m-%d %H:%M:%S", tm_v);
        REQUIRE(sz_v == sz_c);

        REQUIRE(std::string(buf_c, sz_c) == std::string(buf_v, sz_v));
    }

    SECTION("gettime_ts", "[tsc]")
    {
        std::time_t const control = std::time(nullptr);
        std::timespec const variable = tsc::gettime_ts();

        std::tm* tm_c = std::gmtime(&control);
        std::tm* tm_v = std::gmtime(&variable.tv_sec);

        char buf_c[32];
        char buf_v[32];

        std::size_t const sz_c = std::strftime(buf_c, sizeof(buf_c), "%Y-%m-%d %H:%M:%S", tm_c);
        std::size_t const sz_v = std::strftime(buf_v, sizeof(buf_v), "%Y-%m-%d %H:%M:%S", tm_v);
        REQUIRE(sz_v == sz_c);

        REQUIRE(std::string(buf_c, sz_c) == std::string(buf_v, sz_v));
    }
}

TEST_CASE("free functions", "[time]")
{
    SECTION("ts_diff")
    {
        std::timespec t1 = {10, 1000};
        std::timespec t2 = {10, 1500};
        std::timespec diff = {0, 0};

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
        std::timespec ts = {0, 0};

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
        std::timespec ts = {0, 0};

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

    SECTION("to_utc_str")
    {
        REQUIRE(to_utc_str(0) == "1970-01-01 00:00:00.000000000");
        REQUIRE(to_utc_str(420'021'409'000'000'123) == "1983-04-24 08:36:49.420021409");
    }

    SECTION("to_time_str")
    {
        REQUIRE(to_time_str(0) == "00:00:00.000000000");
        REQUIRE(to_time_str(37'921'286'412'345) == "10:32:01.286412345");
    }
}
