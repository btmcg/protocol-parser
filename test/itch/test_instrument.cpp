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
        REQUIRE(i1.book.bids().empty());
        REQUIRE(i1.book.asks().empty());
        REQUIRE(i1.open == 0);
        REQUIRE(i1.close == 0);
        REQUIRE(i1.lo == InvalidLoPrice);
        REQUIRE(i1.hi == InvalidHiPrice);
        REQUIRE(i1.num_trades == 0);
        REQUIRE(i1.trade_qty == 0);
        REQUIRE(i1.num_orders == 0);
        REQUIRE(i1.instrument_state == InstrumentState::Unknown);

        instrument i2(42, "ABCD   ");
        REQUIRE(i2.locate == 42);
        REQUIRE(std::memcmp(i2.name, "ABCD\0\0\0\0", sizeof(i2.name)) == 0);
        REQUIRE(i2.book.bids().empty());
        REQUIRE(i2.book.asks().empty());
        REQUIRE(i2.open == 0);
        REQUIRE(i2.close == 0);
        REQUIRE(i2.lo == InvalidLoPrice);
        REQUIRE(i2.hi == InvalidHiPrice);
        REQUIRE(i2.num_trades == 0);
        REQUIRE(i2.trade_qty == 0);
        REQUIRE(i2.num_orders == 0);
        REQUIRE(i2.instrument_state == InstrumentState::Unknown);
    }

    SECTION("setters")
    {
        instrument i;
        i.set_name("ABCD   ");
        REQUIRE(std::memcmp(i.name, "ABCD\0\0\0\0", sizeof(i.name)) == 0);
    }
}
