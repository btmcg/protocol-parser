#include "itch/price_level.hpp"
#include <catch2/catch.hpp>


TEST_CASE("price_level", "[price_level]")
{
    using namespace itch;
    order o1(Side::Bid, 100'000, 100);
    price_level pl(o1);

    SECTION("init")
    {
        REQUIRE(pl.price() == o1.price);
        REQUIRE(pl.agg_qty() == o1.qty);
        REQUIRE(pl.size() == 1);
        REQUIRE(pl.max_orders() == 1);
        REQUIRE(pl.orders().size() == 1);
        for (auto o : pl.orders())
            REQUIRE(*o == o1); // only one order
    }

    SECTION("add_order")
    {
        // at the start, the price_level looks like:
        //  [price=100'000, agg_qty=100]
        //      -> 100 (o1)

        order o2(Side::Bid, o1.price, 200);
        pl.add_order(o2);
        // price_level should look like
        //  [price=100'000, agg_qty=300]
        //      -> 100 (o1)
        //      -> 200 (o2)
        REQUIRE(pl.price() == o1.price);
        REQUIRE(pl.agg_qty() == o1.qty + o2.qty);
        REQUIRE(pl.size() == 2);
        REQUIRE(pl.max_orders() == 2);

        order o3(Side::Bid, o1.price, 300);
        pl.add_order(o3);
        // price_level should look like
        //  [price=100'000, agg_qty=600]
        //      -> 100 (o1)
        //      -> 200 (o2)
        //      -> 300 (o3)
        REQUIRE(pl.price() == o1.price);
        REQUIRE(pl.agg_qty() == o1.qty + o2.qty + o3.qty);
        REQUIRE(pl.size() == 3);
        REQUIRE(pl.max_orders() == 3);

        order o4(Side::Bid, o1.price, 50);
        pl.add_order(o4);
        // price_level should look like
        //  [price=100'000, agg_qty=650]
        //      -> 100 (o1)
        //      -> 200 (o2)
        //      -> 300 (o3)
        //      -> 50  (o4)
        REQUIRE(pl.price() == o1.price);
        REQUIRE(pl.agg_qty() == o1.qty + o2.qty + o3.qty + o4.qty);
        REQUIRE(pl.size() == 4);
        REQUIRE(pl.max_orders() == 4);

        // make sure the original order is still at begin()
        // clang-format off
        auto o_itr = pl.orders().begin();
        REQUIRE((*o_itr)->qty == o1.qty); ++o_itr;
        REQUIRE((*o_itr)->qty == o2.qty); ++o_itr;
        REQUIRE((*o_itr)->qty == o3.qty); ++o_itr;
        REQUIRE((*o_itr)->qty == o4.qty); ++o_itr;
        REQUIRE(o_itr == pl.orders().end());
        // clang-format on
    }

    SECTION("delete_order")
    {
        // at start, price_level should look like
        //  [price=100'000, agg_qty=100]
        //      -> 100 (o1)

        order o2(Side::Bid, o1.price, 200);
        pl.add_order(o2);
        order o3(Side::Bid, o1.price, 300);
        pl.add_order(o3);
        order o4(Side::Bid, o1.price, 50);
        pl.add_order(o4);

        // price_level should look like
        //  [price=100'000, agg_qty=650]
        //      -> 100 (o1)
        //      -> 200 (o2)
        //      -> 300 (o3)
        //      -> 50  (o4)
        REQUIRE(pl.price() == o1.price);
        REQUIRE(pl.agg_qty() == o1.qty + o2.qty + o3.qty + o4.qty);
        REQUIRE(pl.size() == 4);
        REQUIRE(pl.max_orders() == 4);

        pl.delete_order(o3);
        // price_level should now look like
        //  [price=100'000, agg_qty=350]
        //      -> 100 (o1)
        //      -> 200 (o2)
        //      -> 50  (o4)
        REQUIRE(pl.price() == o1.price);
        REQUIRE(pl.agg_qty() == o1.qty + o2.qty + o4.qty);
        REQUIRE(pl.size() == 3);
        REQUIRE(pl.max_orders() == 4);
        {
            // clang-format off
            auto o_itr = pl.orders().begin();
            REQUIRE((*o_itr)->qty == o1.qty); ++o_itr;
            REQUIRE((*o_itr)->qty == o2.qty); ++o_itr;
            REQUIRE((*o_itr)->qty == o4.qty); ++o_itr;
            REQUIRE(o_itr == pl.orders().end());
            // clang-format on
        }

        pl.delete_order(o2);
        // price_level should now look like
        //  [price=100'000, agg_qty=150]
        //      -> 100 (o1)
        //      -> 50  (o4)
        REQUIRE(pl.price() == o1.price);
        REQUIRE(pl.agg_qty() == o1.qty + o4.qty);
        REQUIRE(pl.size() == 2);
        REQUIRE(pl.max_orders() == 4);
        {
            // clang-format off
            auto o_itr = pl.orders().begin();
            REQUIRE((*o_itr)->qty == o1.qty); ++o_itr;
            REQUIRE((*o_itr)->qty == o4.qty); ++o_itr;
            REQUIRE(o_itr == pl.orders().end());
            // clang-format on
        }

        pl.delete_order(o1);
        // price_level should now look like
        //  [price=100'000, agg_qty=50]
        //      -> 50  (o4)
        REQUIRE(pl.price() == o1.price);
        REQUIRE(pl.agg_qty() == o4.qty);
        REQUIRE(pl.size() == 1);
        REQUIRE(pl.max_orders() == 4);
        {
            // clang-format off
            auto o_itr = pl.orders().begin();
            REQUIRE((*o_itr)->qty == o4.qty); ++o_itr;
            REQUIRE(o_itr == pl.orders().end());
            // clang-format on
        }

        pl.delete_order(o4);
        // price_level should now be empty
        REQUIRE(pl.price() == o1.price);
        REQUIRE(pl.agg_qty() == 0);
        REQUIRE(pl.size() == 0);
        REQUIRE(pl.max_orders() == 4);

        REQUIRE(pl.orders().begin() == pl.orders().end());
    }

    SECTION("cancel_order")
    {
        // at start, price_level should look like
        //  [price=100'000, agg_qty=100]
        //      -> 100 (o1)

        order o2(Side::Bid, o1.price, 200);
        pl.add_order(o2);
        order o3(Side::Bid, o1.price, 300);
        pl.add_order(o3);
        order o4(Side::Bid, o1.price, 50);
        pl.add_order(o4);

        // price_level should now look like
        //  [price=100'000, agg_qty=650]
        //      -> 100 (o1)
        //      -> 200 (o2)
        //      -> 300 (o3)
        //      -> 50  (o4)
        REQUIRE(pl.price() == o1.price);
        REQUIRE(pl.agg_qty() == o1.qty + o2.qty + o3.qty + o4.qty);
        REQUIRE(pl.size() == 4);
        REQUIRE(pl.max_orders() == 4);

        qty_t cancel_qty = 50;
        o3.qty -= cancel_qty;
        pl.cancel_order(o3, cancel_qty);
        // price_level should now look like
        //  [price=100'000, agg_qty=600]
        //      -> 100 (o1)
        //      -> 200 (o2)
        //      -> 250 (o3)
        //      -> 50  (o4)
        REQUIRE(pl.price() == o1.price);
        REQUIRE(pl.agg_qty() == o1.qty + o2.qty + o3.qty + o4.qty);
        REQUIRE(pl.size() == 4);
        REQUIRE(pl.max_orders() == 4);
        {
            // clang-format off
            auto o_itr = pl.orders().begin();
            REQUIRE((*o_itr)->qty == o1.qty); ++o_itr;
            REQUIRE((*o_itr)->qty == o2.qty); ++o_itr;
            REQUIRE((*o_itr)->qty == o3.qty); ++o_itr;
            REQUIRE((*o_itr)->qty == o4.qty); ++o_itr;
            REQUIRE(o_itr == pl.orders().end());
            // clang-format on
        }

        cancel_qty = 125;
        o2.qty -= cancel_qty;
        pl.cancel_order(o2, cancel_qty);
        // price_level should now look like
        //  [price=100'000, agg_qty=475]
        //      -> 100 (o1)
        //      -> 25  (o2)
        //      -> 250 (o3)
        //      -> 50  (o4)
        REQUIRE(pl.price() == o1.price);
        REQUIRE(pl.agg_qty() == o1.qty + o2.qty + o3.qty + o4.qty);
        REQUIRE(pl.size() == 4);
        REQUIRE(pl.max_orders() == 4);
        {
            // clang-format off
            auto o_itr = pl.orders().begin();
            REQUIRE((*o_itr)->qty == o1.qty); ++o_itr;
            REQUIRE((*o_itr)->qty == o2.qty); ++o_itr;
            REQUIRE((*o_itr)->qty == o3.qty); ++o_itr;
            REQUIRE((*o_itr)->qty == o4.qty); ++o_itr;
            REQUIRE(o_itr == pl.orders().end());
            // clang-format on
        }
    }
}
