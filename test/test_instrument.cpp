#include "common/instrument.hpp"
#include <catch.hpp>
#include <cstring>


TEST_CASE("initial state", "[instrument]")
{
    SECTION("constructors")
    {
        instrument i1;
        REQUIRE(i1.locate == 0);
        REQUIRE(std::memcmp(i1.name, "\0\0\0\0\0\0\0\0", 8) == 0);

        instrument i2(42, "ABCD   ");
        REQUIRE(i2.locate == 42);
        REQUIRE(std::memcmp(i2.name, "ABCD\0\0\0\0", 8) == 0);
    }
}
