#include "itch/hashed_book.hpp"
#include <catch2/catch.hpp>


TEST_CASE("hashed_book", "[hashed_book]")
{
    using namespace itch;
    hashed_book book;

    SECTION("add_order bids")
    {
        order o1(Side::Bid, 10000, 100);
        order o2(Side::Bid, 20000, 200);
        order o3(Side::Bid, 30000, 300);
        order o4(Side::Bid, 20000, 500);
        order o5(Side::Bid, 30000, 500);

        book.add_order(o1);
        REQUIRE(o1.pl->price() == o1.price);
        REQUIRE(o1.pl->agg_qty() == o1.qty);
        REQUIRE(book.best_bid().price == o1.price);
        REQUIRE(book.best_bid().qty == o1.qty);

        book.add_order(o2);
        REQUIRE(o2.pl->price() == o2.price);
        REQUIRE(o2.pl->agg_qty() == o2.qty);
        REQUIRE(book.best_bid().price == o2.price);
        REQUIRE(book.best_bid().qty == o2.qty);

        book.add_order(o3);
        REQUIRE(o3.pl->price() == o3.price);
        REQUIRE(o3.pl->agg_qty() == o3.qty);
        REQUIRE(book.best_bid().price == o3.price);
        REQUIRE(book.best_bid().qty == o3.qty);

        REQUIRE(book.bids().size() == 3);

        // new order in middle of book
        book.add_order(o4);
        REQUIRE(o4.pl->price() == o4.price);
        REQUIRE(o4.pl->agg_qty() == o4.qty + o2.qty);
        REQUIRE(book.bids().size() == 3);

        // new inside bid
        book.add_order(o5);
        REQUIRE(o5.pl->price() == o5.price);
        REQUIRE(o5.pl->agg_qty() == o3.qty + o5.qty);
        REQUIRE(book.best_bid().price == o5.price);
        REQUIRE(book.best_bid().qty == o3.qty + o5.qty);
        REQUIRE(book.bids().size() == 3);
    }

    SECTION("add_order bids random")
    {
        order o1(Side::Bid, 10000, 100);
        order o2(Side::Bid, 20000, 200);
        order o3(Side::Bid, 30000, 300);
        order o4(Side::Bid, 20000, 500);
        order o5(Side::Bid, 30000, 500);

        // final bid book should look like:
        //  800 @ 30000
        //  700 @ 20000
        //  100 @ 10000

        book.add_order(o1);
        book.add_order(o2);
        book.add_order(o3);
        book.add_order(o4);
        book.add_order(o5);

        hashed_book book2;
        book2.add_order(o5);
        book2.add_order(o4);
        book2.add_order(o3);
        book2.add_order(o2);
        book2.add_order(o1);

        hashed_book book3;
        book3.add_order(o3);
        book3.add_order(o1);
        book3.add_order(o5);
        book3.add_order(o2);
        book3.add_order(o4);

        REQUIRE(book.best_bid() == book2.best_bid());
        REQUIRE(book2.best_bid() == book3.best_bid());
        REQUIRE(book.bids() == book2.bids());
        REQUIRE(book2.bids() == book3.bids());
    }

    SECTION("add_order asks random")
    {
        order o1(Side::Ask, 10000, 100);
        order o2(Side::Ask, 20000, 200);
        order o3(Side::Ask, 30000, 300);
        order o4(Side::Ask, 20000, 500);
        order o5(Side::Ask, 30000, 500);

        // final ask book should look like:
        //  100 @ 10000
        //  700 @ 20000
        //  800 @ 30000

        book.add_order(o1);
        book.add_order(o2);
        book.add_order(o3);
        book.add_order(o4);
        book.add_order(o5);

        hashed_book book2;
        book2.add_order(o5);
        book2.add_order(o4);
        book2.add_order(o3);
        book2.add_order(o2);
        book2.add_order(o1);

        hashed_book book3;
        book3.add_order(o3);
        book3.add_order(o1);
        book3.add_order(o5);
        book3.add_order(o2);
        book3.add_order(o4);

        REQUIRE(book.best_ask() == book2.best_ask());
        REQUIRE(book2.best_ask() == book3.best_ask());
        REQUIRE(book.asks() == book2.asks());
        REQUIRE(book2.asks() == book3.asks());
    }


    SECTION("delete_order")
    {
        order o1(Side::Bid, 100, 100);
        order o2(Side::Bid, 200, 200);
        order o3(Side::Bid, 300, 300);
        order o4(Side::Bid, 300, 500);

        book.add_order(o1);
        book.add_order(o2);
        book.add_order(o3);
        book.add_order(o4);

        book.delete_order(o2);
        REQUIRE(book.bids().size() == 2);
        REQUIRE(o2.price == 0);
        REQUIRE(o2.qty == 0);

        book.delete_order(o1);
        REQUIRE(book.bids().size() == 1);
        REQUIRE(o1.price == 0);
        REQUIRE(o1.qty == 0);

        // only deletes part of level
        book.delete_order(o3);
        REQUIRE(book.bids().size() == 1);
        REQUIRE(o3.price == 0);
        REQUIRE(o3.qty == 0);
        REQUIRE(o3.pl == nullptr);
        REQUIRE(book.best_bid().price == o4.price);
        REQUIRE(book.best_bid().qty == o4.qty);
    }

    SECTION("replace_order")
    {
        order o1(Side::Bid, 100, 100);
        order o2(Side::Bid, 200, 200);
        order o3(Side::Bid, 300, 300);
        order o4(Side::Bid, 300, 500);

        book.add_order(o1);
        book.add_order(o2);
        book.add_order(o3);
        book.add_order(o4);

        order new_o2(Side::Bid, 200, 500);
        book.replace_order(o2, new_o2);
        REQUIRE(book.bids().size() == 3);
        REQUIRE(o2.price == 0);
        REQUIRE(o2.qty == 0);
        REQUIRE(o2.pl == nullptr);
        REQUIRE(new_o2.price == 200);
        REQUIRE(new_o2.qty == 500);
        REQUIRE(new_o2.pl->price() == 200);
        REQUIRE(new_o2.pl->agg_qty() == 500);

        // replace part of level
        order new_o3(Side::Bid, 300, 500);
        book.replace_order(o3, new_o3);
        REQUIRE(book.best_bid().price == 300);
        REQUIRE(book.best_bid().qty == 1000);
        REQUIRE(o3.price == 0);
        REQUIRE(o3.qty == 0);
        REQUIRE(o3.pl == nullptr);
        REQUIRE(new_o3.price == 300);
        REQUIRE(new_o3.qty == 500);
        REQUIRE(new_o3.pl->price() == 300);
        REQUIRE(new_o3.pl->agg_qty() == 1000);
    }

    SECTION("cancel_order")
    {
        order o1(Side::Bid, 100, 100);
        order o2(Side::Bid, 200, 200);
        order o3(Side::Bid, 300, 300);
        order o4(Side::Bid, 300, 500);

        book.add_order(o1);
        book.add_order(o2);
        book.add_order(o3);
        book.add_order(o4);

        book.cancel_order(o2, 25);
        REQUIRE(o2.price == 200);
        REQUIRE(o2.qty == 175);
        REQUIRE(o2.pl->price() == o2.price);
        REQUIRE(o2.pl->agg_qty() == o2.qty);
        REQUIRE(book.bids().size() == 3);

        // partial cancel
        book.cancel_order(o1, 50);
        REQUIRE(o1.price == 100);
        REQUIRE(o1.qty == 50);
        REQUIRE(o1.pl->price() == 100);
        REQUIRE(o1.pl->agg_qty() == 50);
        REQUIRE(book.bids().size() == 3);

        // partial cancel on inside
        book.cancel_order(o4, 50);
        REQUIRE(o4.price == 300);
        REQUIRE(o4.qty == 450);
        REQUIRE(o4.pl->price() == 300);
        REQUIRE(o4.pl->agg_qty() == 750);
        REQUIRE(book.bids().size() == 3);
        REQUIRE(book.best_bid().price == 300);
        REQUIRE(book.best_bid().qty == 750);
    }

    SECTION("bids")
    {
        order o1(Side::Bid, 100, 10);
        order o2(Side::Bid, 200, 20);
        order o3(Side::Bid, 300, 30);
        order o4(Side::Bid, 400, 40);
        order o5(Side::Bid, 500, 50);

        book.add_order(o3);
        book.add_order(o2);
        book.add_order(o5);
        book.add_order(o1);
        book.add_order(o4);

        auto const& bids = book.bids();
        REQUIRE(bids.size() == 5);

        auto itr = bids.cbegin();
        REQUIRE(itr->price() == 500);
        REQUIRE(itr->agg_qty() == 50);
        ++itr;
        REQUIRE(itr->price() == 400);
        REQUIRE(itr->agg_qty() == 40);
        ++itr;
        REQUIRE(itr->price() == 300);
        REQUIRE(itr->agg_qty() == 30);
        ++itr;
        REQUIRE(itr->price() == 200);
        REQUIRE(itr->agg_qty() == 20);
        ++itr;
        REQUIRE(itr->price() == 100);
        REQUIRE(itr->agg_qty() == 10);
    }

    SECTION("asks")
    {
        order o1(Side::Ask, 100, 10);
        order o2(Side::Ask, 200, 20);
        order o3(Side::Ask, 300, 30);
        order o4(Side::Ask, 400, 40);
        order o5(Side::Ask, 500, 50);

        book.add_order(o3);
        book.add_order(o2);
        book.add_order(o5);
        book.add_order(o1);
        book.add_order(o4);

        auto const& asks = book.asks();
        REQUIRE(asks.size() == 5);

        auto itr = asks.cbegin();
        REQUIRE(itr->price() == 100);
        REQUIRE(itr->agg_qty() == 10);
        ++itr;
        REQUIRE(itr->price() == 200);
        REQUIRE(itr->agg_qty() == 20);
        ++itr;
        REQUIRE(itr->price() == 300);
        REQUIRE(itr->agg_qty() == 30);
        ++itr;
        REQUIRE(itr->price() == 400);
        REQUIRE(itr->agg_qty() == 40);
        ++itr;
        REQUIRE(itr->price() == 500);
        REQUIRE(itr->agg_qty() == 50);
    }
}
