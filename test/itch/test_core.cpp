#include "itch/core.hpp"
#include <catch2/catch.hpp>


TEST_CASE("order", "[order]")
{
    using namespace itch;

    SECTION("size")
    {
        REQUIRE(sizeof(order) == 32);
    }

    SECTION("default ctor")
    {
        order o1;
        REQUIRE(o1.price == 0);
        REQUIRE(o1.qty == 0);
        REQUIRE_FALSE(o1.valid_pl);
        REQUIRE(o1.ts == 0);
        REQUIRE(o1.side == Side::Bid);
    }

    SECTION("value ctor")
    {
        order o1(Side::Ask, 45000, 100);
        REQUIRE(o1.price == 45000);
        REQUIRE(o1.qty == 100);
        REQUIRE_FALSE(o1.valid_pl);
        REQUIRE(o1.ts == 0);
        REQUIRE(o1.side == Side::Ask);
    }

    SECTION("copy ctor")
    {
        order o1(Side::Ask, 12345, 100);
        order o2(o1);
        REQUIRE(o2.price == 12345);
        REQUIRE(o2.qty == 100);
        REQUIRE_FALSE(o2.valid_pl);
        REQUIRE(o2.ts == 0);
        REQUIRE(o2.side == Side::Ask);
    }

    SECTION("move ctor")
    {
        order o1(Side::Ask, 12345, 100);
        order o2(std::move(o1));
        REQUIRE(o2.price == 12345);
        REQUIRE(o2.qty == 100);
        REQUIRE_FALSE(o2.valid_pl);
        REQUIRE(o2.ts == 0);
        REQUIRE(o2.side == Side::Ask);
    }

    SECTION("copy assignment")
    {
        order o1(Side::Ask, 12345, 100);
        order o2(Side::Bid, 67890, 200);
        o2 = o1;
        REQUIRE(o2.price == 12345);
        REQUIRE(o2.qty == 100);
        REQUIRE_FALSE(o2.valid_pl);
        REQUIRE(o2.ts == 0);
        REQUIRE(o2.side == Side::Ask);
    }

    SECTION("move assignment")
    {
        order o1(Side::Ask, 12345, 100);
        order o2(Side::Bid, 67890, 200);
        o2 = std::move(o1);
        REQUIRE(o2.price == 12345);
        REQUIRE(o2.qty == 100);
        REQUIRE_FALSE(o2.valid_pl);
        REQUIRE(o2.ts == 0);
        REQUIRE(o2.side == Side::Ask);
    }
}
