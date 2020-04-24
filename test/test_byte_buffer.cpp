#include "parser/byte_buffer.hpp"
#include <catch.hpp>


TEST_CASE("empty bb", "[byte_buffer]")
{
    byte_buffer<10> buf;

    SECTION("const functions")
    {
        REQUIRE(buf.capacity() == 10);
    }
}
