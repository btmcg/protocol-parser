#include "itch/instrument.hpp"
#include <catch2/catch.hpp>
#include <cstring>


TEST_CASE("initial state", "[instrument]")
{
    using namespace itch;

    SECTION("constructors")
    {
        instrument i1;
        REQUIRE(i1.locate == 0);
        REQUIRE(std::memcmp(i1.name, "\0\0\0\0\0\0\0\0", sizeof(i1.name)) == 0);

        instrument i2(42, "ABCD   ");
        REQUIRE(i2.locate == 42);
        REQUIRE(std::memcmp(i2.name, "ABCD\0\0\0\0", sizeof(i2.name)) == 0);
    }

    SECTION("setters")
    {
        instrument i;
        i.set_name("ABCD   ");
        REQUIRE(std::memcmp(i.name, "ABCD\0\0\0\0", sizeof(i.name)) == 0);
    }
}
