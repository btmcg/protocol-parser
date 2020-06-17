#include "itch/hashed_book.hpp"
#include <catch2/catch.hpp>


TEST_CASE("hashed_book", "[hashed_book]")
{
    using namespace itch;
    hashed_book book;

    SECTION("add_order bids")
    {
        // initial bid establishes price and best bid
        order o1(Side::Bid, 100'000, 100);
        book.add_order(o1);
        // bid book should look like:
        //  100 @ 100'000
        //      -> 100 (o1)
        REQUIRE(o1.valid_pl);
        REQUIRE(o1.pl->price() == o1.price);
        REQUIRE(o1.pl->agg_qty() == o1.qty);
        REQUIRE(book.best_bid().price == o1.price);
        REQUIRE(book.best_bid().qty == o1.qty);
        REQUIRE(book.bids().size() == 1);

        // establishes new price and best bid
        order o2(Side::Bid, 200'000, 200);
        book.add_order(o2);
        // bid book should look like:
        //  200 @ 200'000
        //      -> 200 (o2)
        //  100 @ 100'000
        //      -> 100 (o1)
        REQUIRE(o2.valid_pl);
        REQUIRE(o2.pl->price() == o2.price);
        REQUIRE(o2.pl->agg_qty() == o2.qty);
        REQUIRE(book.best_bid().price == o2.price);
        REQUIRE(book.best_bid().qty == o2.qty);
        REQUIRE(book.bids().size() == 2);

        // establishes new price and best bid
        order o3(Side::Bid, 300'000, 300);
        book.add_order(o3);
        // bid book should look like:
        //  300 @ 300'000
        //      -> 300 (o3)
        //  200 @ 200'000
        //      -> 200 (o2)
        //  100 @ 100'000
        //      -> 100 (o1)
        REQUIRE(o3.valid_pl);
        REQUIRE(o3.pl->price() == o3.price);
        REQUIRE(o3.pl->agg_qty() == o3.qty);
        REQUIRE(book.best_bid().price == o3.price);
        REQUIRE(book.best_bid().qty == o3.qty);
        REQUIRE(book.bids().size() == 3);

        // new order in middle of book at already-establised price_level
        order o4(Side::Bid, 200'000, 500);
        book.add_order(o4);
        // bid book should look like:
        //  300 @ 300'000
        //      -> 300 (o3)
        //  700 @ 200'000
        //      -> 200 (o2)
        //      -> 500 (o4)
        //  100 @ 100'000
        //      -> 100 (o1)
        REQUIRE(o4.valid_pl);
        REQUIRE(o4.pl->price() == o4.price);
        REQUIRE(o4.pl->agg_qty() == o2.qty + o4.qty);
        REQUIRE(o4.pl->size() == 2);
        REQUIRE(book.best_bid().price == o3.price);
        REQUIRE(book.best_bid().qty == o3.qty);
        REQUIRE(book.bids().size() == 3);

        // new order at inside price_level
        order o5(Side::Bid, 300'000, 500);
        book.add_order(o5);
        // bid book should look like:
        //  800 @ 300'000
        //      -> 300 (o3)
        //      -> 500 (o5)
        //  700 @ 200'000
        //      -> 200 (o2)
        //      -> 500 (o4)
        //  100 @ 100'000
        //      -> 100 (o1)
        REQUIRE(o5.valid_pl);
        REQUIRE(o5.pl->price() == o5.price);
        REQUIRE(o5.pl->agg_qty() == o3.qty + o5.qty);
        REQUIRE(o5.pl->size() == 2);
        REQUIRE(book.best_bid().price == o5.price);
        REQUIRE(book.best_bid().qty == o3.qty + o5.qty);
        REQUIRE(book.bids().size() == 3);
    }

    SECTION("add_order bids random")
    {
        order o1(Side::Bid, 100'000, 100);
        order o2(Side::Bid, 200'000, 200);
        order o3(Side::Bid, 300'000, 300);
        order o4(Side::Bid, 200'000, 500);
        order o5(Side::Bid, 300'000, 500);

        book.add_order(o1);
        book.add_order(o2);
        book.add_order(o3);
        book.add_order(o4);
        book.add_order(o5);
        // book should look like:
        //  800 @ 300'000
        //      -> 300 (o3)
        //      -> 500 (o5)
        //  700 @ 200'000
        //      -> 200 (o2)
        //      -> 500 (o4)
        //  100 @ 100'000
        //      -> 100 (o1)

        hashed_book book2;
        book2.add_order(o5);
        book2.add_order(o4);
        book2.add_order(o3);
        book2.add_order(o2);
        book2.add_order(o1);
        // book2 should look like:
        //  800 @ 300'000
        //      -> 500 (o5)
        //      -> 300 (o3)
        //  700 @ 200'000
        //      -> 500 (o4)
        //      -> 200 (o2)
        //  100 @ 100'000
        //      -> 100 (o1)

        hashed_book book3;
        book3.add_order(o3);
        book3.add_order(o1);
        book3.add_order(o5);
        book3.add_order(o4);
        book3.add_order(o2);
        // book3 should look like:
        //  800 @ 300'000
        //      -> 300 (o3)
        //      -> 500 (o5)
        //  700 @ 200'000
        //      -> 500 (o4)
        //      -> 200 (o2)
        //  100 @ 100'000
        //      -> 100 (o1)

        // best bid should be the same regardless of order
        REQUIRE(book.best_bid() == book2.best_bid());
        REQUIRE(book2.best_bid() == book3.best_bid());

        // however, the orders in each price level will not be
        // consistent
        REQUIRE(book.bids() != book2.bids());
        REQUIRE(book.bids() != book3.bids());
        REQUIRE(book2.bids() != book3.bids());
    }

    SECTION("add_order asks")
    {
        // initial ask establishes price and best ask
        order o1(Side::Ask, 200'000, 100);
        book.add_order(o1);
        // ask book should look like:
        //  100 @ 200'000
        //      -> 100 (o1)
        REQUIRE(o1.valid_pl);
        REQUIRE(o1.pl->price() == o1.price);
        REQUIRE(o1.pl->agg_qty() == o1.qty);
        REQUIRE(book.best_ask().price == o1.price);
        REQUIRE(book.best_ask().qty == o1.qty);
        REQUIRE(book.asks().size() == 1);

        // establishes new price at bottom of book
        order o2(Side::Ask, 300'000, 200);
        book.add_order(o2);
        // ask book should look like:
        //  100 @ 200'000
        //      -> 100 (o1)
        //  200 @ 300'000
        //      -> 200 (o2)
        REQUIRE(o2.valid_pl);
        REQUIRE(o2.pl->price() == o2.price);
        REQUIRE(o2.pl->agg_qty() == o2.qty);
        REQUIRE(book.best_ask().price == o1.price);
        REQUIRE(book.best_ask().qty == o1.qty);
        REQUIRE(book.asks().size() == 2);

        // establishes new price and best ask
        order o3(Side::Ask, 100'000, 300);
        book.add_order(o3);
        // ask book should look like:
        //  300 @ 100'000
        //      -> 300 (o3)
        //  100 @ 200'000
        //      -> 200 (o1)
        //  200 @ 300'000
        //      -> 200 (o2)
        REQUIRE(o3.valid_pl);
        REQUIRE(o3.pl->price() == o3.price);
        REQUIRE(o3.pl->agg_qty() == o3.qty);
        REQUIRE(book.best_ask().price == o3.price);
        REQUIRE(book.best_ask().qty == o3.qty);
        REQUIRE(book.asks().size() == 3);

        // new order in middle of book at already-establised price_level
        order o4(Side::Ask, 200'000, 500);
        book.add_order(o4);
        // ask book should look like:
        //  300 @ 100'000
        //      -> 300 (o3)
        //  700 @ 200'000
        //      -> 200 (o1)
        //      -> 500 (o4)
        //  200 @ 300'000
        //      -> 200 (o2)
        REQUIRE(o4.valid_pl);
        REQUIRE(o4.pl->price() == o4.price);
        REQUIRE(o4.pl->agg_qty() == o1.qty + o4.qty);
        REQUIRE(o4.pl->size() == 2);
        REQUIRE(book.best_ask().price == o3.price);
        REQUIRE(book.best_ask().qty == o3.qty);
        REQUIRE(book.asks().size() == 3);

        // new order at inside price_level
        order o5(Side::Ask, 100'000, 500);
        book.add_order(o5);
        // ask book should look like:
        //  800 @ 100'000
        //      -> 300 (o3)
        //      -> 500 (o5)
        //  700 @ 200'000
        //      -> 200 (o1)
        //      -> 500 (o4)
        //  200 @ 300'000
        //      -> 200 (o2)
        REQUIRE(o5.valid_pl);
        REQUIRE(o5.pl->price() == o5.price);
        REQUIRE(o5.pl->agg_qty() == o3.qty + o5.qty);
        REQUIRE(o5.pl->size() == 2);
        REQUIRE(book.best_ask().price == o3.price);
        REQUIRE(book.best_ask().qty == o3.qty + o5.qty);
        REQUIRE(book.asks().size() == 3);
    }

    SECTION("add_order asks random")
    {
        order o1(Side::Ask, 100'000, 100);
        order o2(Side::Ask, 200'000, 200);
        order o3(Side::Ask, 300'000, 300);
        order o4(Side::Ask, 200'000, 500);
        order o5(Side::Ask, 300'000, 500);

        book.add_order(o1);
        book.add_order(o2);
        book.add_order(o3);
        book.add_order(o4);
        book.add_order(o5);
        // book should look like:
        //  100 @ 100'000
        //      -> 100 (o1)
        //  700 @ 200'000
        //      -> 200 (o2)
        //      -> 500 (o4)
        //  800 @ 300'000
        //      -> 300 (o3)
        //      -> 500 (o5)

        hashed_book book2;
        book2.add_order(o5);
        book2.add_order(o4);
        book2.add_order(o3);
        book2.add_order(o2);
        book2.add_order(o1);
        // book2 should look like:
        //  100 @ 100'000
        //      -> 100 (o1)
        //  700 @ 200'000
        //      -> 500 (o4)
        //      -> 200 (o2)
        //  800 @ 300'000
        //      -> 500 (o5)
        //      -> 300 (o3)

        hashed_book book3;
        book3.add_order(o5);
        book3.add_order(o3);
        book3.add_order(o1);
        book3.add_order(o2);
        book3.add_order(o4);
        // book3 should look like:
        //  100 @ 100'000
        //      -> 100 (o1)
        //  700 @ 200'000
        //      -> 200 (o2)
        //      -> 500 (o4)
        //  800 @ 300'000
        //      -> 500 (o5)
        //      -> 300 (o3)

        // best ask shouldn't change
        REQUIRE(book.best_ask() == book2.best_ask());
        REQUIRE(book2.best_ask() == book3.best_ask());

        // however, the orders in each price level will not be
        // consistent
        REQUIRE(book.asks() != book2.asks());
        REQUIRE(book.asks() != book3.asks());
        REQUIRE(book2.asks() != book3.asks());
    }

    SECTION("delete_order bids")
    {
        order o1(Side::Bid, 100'000, 100);
        book.add_order(o1);
        order o2(Side::Bid, 200'000, 200);
        book.add_order(o2);
        order o3(Side::Bid, 300'000, 300);
        book.add_order(o3);
        order o4(Side::Bid, 300'000, 500);
        book.add_order(o4);
        // current book:
        //  800 @ 300'000
        //      -> 300 (o3)
        //      -> 500 (o4)
        //  200 @ 200'000
        //      -> 200 (o2)
        //  100 @ 100'000
        //      -> 100 (o1)
        REQUIRE(book.bids().size() == 3);
        REQUIRE(book.best_bid().price == o3.price);
        REQUIRE(book.best_bid().qty == o3.qty + o4.qty);

        // deleting o2; since it's the only order on the price_level
        // should remove the price_level as well
        book.delete_order(o2);
        // after delete:
        //  800 @ 300'000
        //      -> 300 (o3)
        //      -> 500 (o4)
        //  100 @ 100'000
        //      -> 100 (o1)
        REQUIRE(book.bids().size() == 2);
        REQUIRE(book.best_bid().price == o3.price);
        REQUIRE(book.best_bid().qty == o3.qty + o4.qty);
        REQUIRE(o2.price == 0);
        REQUIRE(o2.qty == 0);
        REQUIRE_FALSE(o2.valid_pl);

        // delete one order on inside, level remains
        book.delete_order(o3);
        // after delete:
        //  500 @ 300'000
        //      -> 500 (o4)
        //  100 @ 100'000
        //      -> 100 (o1)
        REQUIRE(book.bids().size() == 2);
        REQUIRE(book.best_bid().price == o4.price);
        REQUIRE(book.best_bid().qty == o4.qty);
        REQUIRE(o3.price == 0);
        REQUIRE(o3.qty == 0);
        REQUIRE_FALSE(o3.valid_pl);

        // deletes final order from inside, removing best bid
        book.delete_order(o4);
        // after delete:
        //  100 @ 100'000
        //      -> 100 (o1)
        REQUIRE(book.bids().size() == 1);
        REQUIRE(book.best_bid().price == o1.price);
        REQUIRE(book.best_bid().qty == o1.qty);
        REQUIRE(o4.price == 0);
        REQUIRE(o4.qty == 0);
        REQUIRE_FALSE(o4.valid_pl);

        // delete last order in the book
        book.delete_order(o1);
        REQUIRE(book.bids().size() == 0);
        REQUIRE(book.bids().empty());
        REQUIRE(book.best_bid().price == 0);
        REQUIRE(book.best_bid().qty == 0);
        REQUIRE(o1.price == 0);
        REQUIRE(o1.qty == 0);
        REQUIRE_FALSE(o1.valid_pl);
    }

    SECTION("delete_order asks")
    {
        order o1(Side::Ask, 100'000, 100);
        book.add_order(o1);
        order o2(Side::Ask, 200'000, 200);
        book.add_order(o2);
        order o3(Side::Ask, 100'000, 300);
        book.add_order(o3);
        order o4(Side::Ask, 300'000, 500);
        book.add_order(o4);
        // current book:
        //  400 @ 100'000
        //      -> 100 (o1)
        //      -> 300 (o3)
        //  200 @ 200'000
        //      -> 200 (o2)
        //  500 @ 100'000
        //      -> 500 (o4)
        REQUIRE(book.asks().size() == 3);
        REQUIRE(book.best_ask().price == o1.price);
        REQUIRE(book.best_ask().qty == o1.qty + o3.qty);

        // deleting o2; since it's the only order on the price_level
        // should remove the price_level as well
        book.delete_order(o2);
        // after delete:
        //  400 @ 100'000
        //      -> 100 (o1)
        //      -> 300 (o3)
        //  500 @ 100'000
        //      -> 500 (o4)
        REQUIRE(book.asks().size() == 2);
        REQUIRE(book.best_ask().price == o1.price);
        REQUIRE(book.best_ask().qty == o1.qty + o3.qty);
        REQUIRE(o2.price == 0);
        REQUIRE(o2.qty == 0);
        REQUIRE_FALSE(o2.valid_pl);

        // delete one order on inside, level remains
        book.delete_order(o1);
        // after delete:
        //  400 @ 100'000
        //      -> 300 (o3)
        //  500 @ 100'000
        //      -> 500 (o4)
        REQUIRE(book.asks().size() == 2);
        REQUIRE(book.best_ask().price == o3.price);
        REQUIRE(book.best_ask().qty == o3.qty);
        REQUIRE(o1.price == 0);
        REQUIRE(o1.qty == 0);
        REQUIRE_FALSE(o1.valid_pl);

        // deletes final order from inside, removing best ask
        book.delete_order(o3);
        // after delete:
        //  500 @ 100'000
        //      -> 500 (o4)
        REQUIRE(book.asks().size() == 1);
        REQUIRE(book.best_ask().price == o4.price);
        REQUIRE(book.best_ask().qty == o4.qty);
        REQUIRE(o3.price == 0);
        REQUIRE(o3.qty == 0);
        REQUIRE_FALSE(o3.valid_pl);

        // delete last order in the book
        book.delete_order(o4);
        REQUIRE(book.asks().size() == 0);
        REQUIRE(book.asks().empty());
        REQUIRE(book.best_ask().price == 0);
        REQUIRE(book.best_ask().qty == 0);
        REQUIRE(o4.price == 0);
        REQUIRE(o4.qty == 0);
        REQUIRE_FALSE(o4.valid_pl);
    }

    SECTION("replace_order")
    {
        order o1(Side::Bid, 100'000, 100);
        book.add_order(o1);
        order o2(Side::Bid, 200'000, 200);
        book.add_order(o2);
        order o3(Side::Bid, 300'000, 300);
        book.add_order(o3);
        order o4(Side::Bid, 300'000, 500);
        book.add_order(o4);
        // current book:
        //  800 @ 300'000
        //      -> 300 (o3)
        //      -> 500 (o4)
        //  200 @ 200'000
        //      -> 200 (o2)
        //  100 @ 100'000
        //      -> 100 (o1)

        order new_o2(Side::Bid, 200'000, 500);
        book.replace_order(o2, new_o2);
        // after replace:
        //  800 @ 300'000
        //      -> 300 (o3)
        //      -> 500 (o4)
        //  500 @ 200'000
        //      -> 500 (new_o2)
        //  100 @ 100'000
        //      -> 100 (o1)
        REQUIRE_FALSE(o2.valid_pl);
        REQUIRE(o2.price == 0);
        REQUIRE(o2.qty == 0);
        REQUIRE(new_o2.valid_pl);
        REQUIRE(new_o2.pl->price() == new_o2.price);
        REQUIRE(new_o2.pl->agg_qty() == new_o2.qty);
        REQUIRE(book.best_bid().price == o3.price);
        REQUIRE(book.best_bid().qty == o3.qty + o4.qty);
        REQUIRE(book.bids().size() == 3);

        // replace part of level
        order new_o3(Side::Bid, 300'000, 500);
        book.replace_order(o3, new_o3);
        // after replace:
        //  1000 @ 300'000
        //      -> 500 (o4)
        //      -> 500 (new_o3)
        //  500 @ 200'000
        //      -> 500 (o2)
        //  100 @ 100'000
        //      -> 100 (o1)
        REQUIRE_FALSE(o3.valid_pl);
        REQUIRE(o3.price == 0);
        REQUIRE(o3.qty == 0);
        REQUIRE(new_o3.valid_pl);
        REQUIRE(new_o3.pl->price() == new_o3.price);
        REQUIRE(new_o3.pl->agg_qty() == o4.qty + new_o3.qty);
        REQUIRE(book.best_bid().price == o4.price);
        REQUIRE(book.best_bid().qty == o4.qty + new_o3.qty);
        REQUIRE(book.bids().size() == 3);
    }

    SECTION("cancel_order")
    {
        order o1(Side::Bid, 100'000, 100);
        book.add_order(o1);
        order o2(Side::Bid, 200'000, 200);
        book.add_order(o2);
        order o3(Side::Bid, 300'000, 300);
        book.add_order(o3);
        order o4(Side::Bid, 300'000, 500);
        book.add_order(o4);
        // current book:
        //  800 @ 300'000
        //      -> 300 (o3)
        //      -> 500 (o4)
        //  200 @ 200'000
        //      -> 200 (o2)
        //  100 @ 100'000
        //      -> 100 (o1)

        book.cancel_order(o2, 100);
        // after cancel:
        //  800 @ 300'000
        //      -> 300 (o3)
        //      -> 500 (o4)
        //  100 @ 200'000
        //      -> 100 (o2)
        //  100 @ 100'000
        //      -> 100 (o1)
        REQUIRE(o2.price == 200'000);
        REQUIRE(o2.qty == 100);
        REQUIRE(o2.pl->price() == o2.price);
        REQUIRE(o2.pl->agg_qty() == o2.qty);
        REQUIRE(book.bids().size() == 3);

        book.cancel_order(o1, 50);
        // after cancel:
        //  800 @ 300'000
        //      -> 300 (o3)
        //      -> 500 (o4)
        //  100 @ 200'000
        //      -> 100 (o2)
        //   50 @ 100'000
        //      -> 50  (o1)
        REQUIRE(o1.price == 100'000);
        REQUIRE(o1.qty == 50);
        REQUIRE(o1.pl->price() == o1.price);
        REQUIRE(o1.pl->agg_qty() == o1.qty);
        REQUIRE(book.bids().size() == 3);

        // partial cancel on inside
        book.cancel_order(o4, 50);
        // after cancel:
        //  750 @ 300'000
        //      -> 300 (o3)
        //      -> 450 (o4)
        //  100 @ 200'000
        //      -> 100 (o2)
        //   50 @ 100'000
        //      -> 50  (o1)
        REQUIRE(o4.price == 300'000);
        REQUIRE(o4.qty == 450);
        REQUIRE(o4.pl->price() == o4.price);
        REQUIRE(o4.pl->agg_qty() == o3.qty + o4.qty);
        REQUIRE(book.bids().size() == 3);
        REQUIRE(book.best_bid().price == o3.price);
        REQUIRE(book.best_bid().qty == o3.qty + o4.qty);
    }

    SECTION("bids")
    {
        order o1(Side::Bid, 100'000, 10);
        order o2(Side::Bid, 200'000, 20);
        order o3(Side::Bid, 300'000, 30);
        order o4(Side::Bid, 400'000, 40);
        order o5(Side::Bid, 500'000, 50);

        book.add_order(o3);
        book.add_order(o2);
        book.add_order(o5);
        book.add_order(o1);
        book.add_order(o4);

        auto const& bids = book.bids();
        REQUIRE(bids.size() == 5);

        auto itr = bids.cbegin();
        REQUIRE(itr->price() == o5.price);
        REQUIRE(itr->agg_qty() == o5.qty);
        ++itr;
        REQUIRE(itr->price() == o4.price);
        REQUIRE(itr->agg_qty() == o4.qty);
        ++itr;
        REQUIRE(itr->price() == o3.price);
        REQUIRE(itr->agg_qty() == o3.qty);
        ++itr;
        REQUIRE(itr->price() == o2.price);
        REQUIRE(itr->agg_qty() == o2.qty);
        ++itr;
        REQUIRE(itr->price() == o1.price);
        REQUIRE(itr->agg_qty() == o1.qty);
    }

    SECTION("asks")
    {
        order o1(Side::Ask, 100'000, 10);
        order o2(Side::Ask, 200'000, 20);
        order o3(Side::Ask, 300'000, 30);
        order o4(Side::Ask, 400'000, 40);
        order o5(Side::Ask, 500'000, 50);

        book.add_order(o3);
        book.add_order(o2);
        book.add_order(o5);
        book.add_order(o1);
        book.add_order(o4);

        auto const& asks = book.asks();
        REQUIRE(asks.size() == 5);

        auto itr = asks.cbegin();
        REQUIRE(itr->price() == o1.price);
        REQUIRE(itr->agg_qty() == o1.qty);
        ++itr;
        REQUIRE(itr->price() == o2.price);
        REQUIRE(itr->agg_qty() == o2.qty);
        ++itr;
        REQUIRE(itr->price() == o3.price);
        REQUIRE(itr->agg_qty() == o3.qty);
        ++itr;
        REQUIRE(itr->price() == o4.price);
        REQUIRE(itr->agg_qty() == o4.qty);
        ++itr;
        REQUIRE(itr->price() == o5.price);
        REQUIRE(itr->agg_qty() == o5.qty);
    }
}

// #include "itch/core.hpp"
// #include "itch/hashed_book.hpp"
// #include <catch2/catch.hpp>
// #include <cstring>


// TEST_CASE("hashed_book", "[hashed_book]")
// {
//     using namespace itch;
//     hashed_book book;

//     SECTION("add_order bids")
//     {
//         order o1(Side::Bid, 10000, 100);
//         order o2(Side::Bid, 20000, 200);
//         order o3(Side::Bid, 30000, 300);
//         order o4(Side::Bid, 20000, 500);
//         order o5(Side::Bid, 30000, 500);

//         book.add_order(o1);
//         REQUIRE(o1.pl->price == o1.price);
//         REQUIRE(o1.pl->qty == o1.qty);
//         REQUIRE(book.best_bid().price == o1.price);
//         REQUIRE(book.best_bid().qty == o1.qty);

//         book.add_order(o2);
//         REQUIRE(o2.pl->price == o2.price);
//         REQUIRE(o2.pl->qty == o2.qty);
//         REQUIRE(book.best_bid().price == o2.price);
//         REQUIRE(book.best_bid().qty == o2.qty);

//         book.add_order(o3);
//         REQUIRE(o3.pl->price == o3.price);
//         REQUIRE(o3.pl->qty == o3.qty);
//         REQUIRE(book.best_bid().price == o3.price);
//         REQUIRE(book.best_bid().qty == o3.qty);

//         REQUIRE(book.bids().size() == 3);

//         // new order in middle of book
//         book.add_order(o4);
//         REQUIRE(o4.pl->price == o4.price);
//         REQUIRE(o4.pl->qty == o4.qty + o2.qty);
//         REQUIRE(book.bids().size() == 3);

//         // new inside bid
//         book.add_order(o5);
//         REQUIRE(o5.pl->price == o5.price);
//         REQUIRE(o5.pl->qty == o3.qty + o5.qty);
//         REQUIRE(book.best_bid().price == o5.price);
//         REQUIRE(book.best_bid().qty == o3.qty + o5.qty);
//         REQUIRE(book.bids().size() == 3);
//     }

//     SECTION("add_order bids random")
//     {
//         order o1(Side::Bid, 10000, 100);
//         order o2(Side::Bid, 20000, 200);
//         order o3(Side::Bid, 30000, 300);
//         order o4(Side::Bid, 20000, 500);
//         order o5(Side::Bid, 30000, 500);

//         // final bid book should look like:
//         //  800 @ 30000
//         //  700 @ 20000
//         //  100 @ 10000

//         book.add_order(o1);
//         book.add_order(o2);
//         book.add_order(o3);
//         book.add_order(o4);
//         book.add_order(o5);

//         hashed_book book2;
//         book2.add_order(o5);
//         book2.add_order(o4);
//         book2.add_order(o3);
//         book2.add_order(o2);
//         book2.add_order(o1);

//         hashed_book book3;
//         book3.add_order(o3);
//         book3.add_order(o1);
//         book3.add_order(o5);
//         book3.add_order(o2);
//         book3.add_order(o4);

//         REQUIRE(book.best_bid() == book2.best_bid());
//         REQUIRE(book2.best_bid() == book3.best_bid());
//         REQUIRE(book.bids() == book2.bids());
//         REQUIRE(book2.bids() == book3.bids());
//     }

//     SECTION("add_order asks random")
//     {
//         order o1(Side::Ask, 10000, 100);
//         order o2(Side::Ask, 20000, 200);
//         order o3(Side::Ask, 30000, 300);
//         order o4(Side::Ask, 20000, 500);
//         order o5(Side::Ask, 30000, 500);

//         // final ask book should look like:
//         //  100 @ 10000
//         //  700 @ 20000
//         //  800 @ 30000

//         book.add_order(o1);
//         book.add_order(o2);
//         book.add_order(o3);
//         book.add_order(o4);
//         book.add_order(o5);

//         hashed_book book2;
//         book2.add_order(o5);
//         book2.add_order(o4);
//         book2.add_order(o3);
//         book2.add_order(o2);
//         book2.add_order(o1);

//         hashed_book book3;
//         book3.add_order(o3);
//         book3.add_order(o1);
//         book3.add_order(o5);
//         book3.add_order(o2);
//         book3.add_order(o4);

//         REQUIRE(book.best_ask() == book2.best_ask());
//         REQUIRE(book2.best_ask() == book3.best_ask());
//         REQUIRE(book.asks() == book2.asks());
//         REQUIRE(book2.asks() == book3.asks());
//     }


//     SECTION("delete_order")
//     {
//         order o1(Side::Bid, 100, 100);
//         order o2(Side::Bid, 200, 200);
//         order o3(Side::Bid, 300, 300);
//         order o4(Side::Bid, 300, 500);

//         book.add_order(o1);
//         book.add_order(o2);
//         book.add_order(o3);
//         book.add_order(o4);

//         book.delete_order(o2);
//         REQUIRE(book.bids().size() == 2);
//         REQUIRE(o2.price == 0);
//         REQUIRE(o2.qty == 0);

//         // non-existent order
//         order tmp(Side::Bid, 5000, 5000);
//         book.delete_order(tmp);
//         REQUIRE(book.bids().size() == 2);
//         REQUIRE(tmp.price == 5000);
//         REQUIRE(tmp.qty == 5000);
//         REQUIRE(tmp.pl == nullptr);

//         book.delete_order(o1);
//         REQUIRE(book.bids().size() == 1);
//         REQUIRE(o1.price == 0);
//         REQUIRE(o1.qty == 0);

//         // only deletes part of level
//         book.delete_order(o3);
//         REQUIRE(book.bids().size() == 1);
//         REQUIRE(o3.price == 0);
//         REQUIRE(o3.qty == 0);
//         REQUIRE(o3.pl == nullptr);
//         REQUIRE(book.best_bid().price == o4.price);
//         REQUIRE(book.best_bid().qty == o4.qty);
//     }

//     SECTION("replace_order")
//     {
//         order o1(Side::Bid, 100, 100);
//         order o2(Side::Bid, 200, 200);
//         order o3(Side::Bid, 300, 300);
//         order o4(Side::Bid, 300, 500);

//         book.add_order(o1);
//         book.add_order(o2);
//         book.add_order(o3);
//         book.add_order(o4);

//         order new_o2(Side::Bid, 200, 500);
//         book.replace_order(o2, new_o2);
//         REQUIRE(book.bids().size() == 3);
//         REQUIRE(o2.price == 0);
//         REQUIRE(o2.qty == 0);
//         REQUIRE(o2.pl == nullptr);
//         REQUIRE(new_o2.price == 200);
//         REQUIRE(new_o2.qty == 500);
//         REQUIRE(new_o2.pl->price == 200);
//         REQUIRE(new_o2.pl->qty == 500);

//         // replace part of level
//         order new_o3(Side::Bid, 300, 500);
//         book.replace_order(o3, new_o3);
//         REQUIRE(book.best_bid().price == 300);
//         REQUIRE(book.best_bid().qty == 1000);
//         REQUIRE(o3.price == 0);
//         REQUIRE(o3.qty == 0);
//         REQUIRE(o3.pl == nullptr);
//         REQUIRE(new_o3.price == 300);
//         REQUIRE(new_o3.qty == 500);
//         REQUIRE(new_o3.pl->price == 300);
//         REQUIRE(new_o3.pl->qty == 1000);
//     }

//     SECTION("cancel_order")
//     {
//         order o1(Side::Bid, 100, 100);
//         order o2(Side::Bid, 200, 200);
//         order o3(Side::Bid, 300, 300);
//         order o4(Side::Bid, 300, 500);

//         book.add_order(o1);
//         book.add_order(o2);
//         book.add_order(o3);
//         book.add_order(o4);

//         // cancel complete order
//         book.cancel_order(o2, o2.qty);
//         REQUIRE(o2.price == 0);
//         REQUIRE(o2.qty == 0);
//         REQUIRE(o2.pl == nullptr);
//         REQUIRE(book.bids().size() == 2);

//         // partial cancel
//         book.cancel_order(o1, 50);
//         REQUIRE(o1.price == 100);
//         REQUIRE(o1.qty == 50);
//         REQUIRE(o1.pl->price == 100);
//         REQUIRE(o1.pl->qty == 50);
//         REQUIRE(book.bids().size() == 2);

//         // partial cancel on inside
//         book.cancel_order(o4, 50);
//         REQUIRE(o4.price == 300);
//         REQUIRE(o4.qty == 450);
//         REQUIRE(o4.pl->price == 300);
//         REQUIRE(o4.pl->qty == 750);
//         REQUIRE(book.bids().size() == 2);
//         REQUIRE(book.best_bid().price == 300);
//         REQUIRE(book.best_bid().qty == 750);
//     }

//     SECTION("bids")
//     {
//         order o1(Side::Bid, 100, 10);
//         order o2(Side::Bid, 200, 20);
//         order o3(Side::Bid, 300, 30);
//         order o4(Side::Bid, 400, 40);
//         order o5(Side::Bid, 500, 50);

//         book.add_order(o3);
//         book.add_order(o2);
//         book.add_order(o5);
//         book.add_order(o1);
//         book.add_order(o4);

//         auto const& bids = book.bids();
//         REQUIRE(bids.size() == 5);

//         auto itr = bids.cbegin();
//         REQUIRE(itr->price == 500);
//         REQUIRE(itr->qty == 50);
//         ++itr;
//         REQUIRE(itr->price == 400);
//         REQUIRE(itr->qty == 40);
//         ++itr;
//         REQUIRE(itr->price == 300);
//         REQUIRE(itr->qty == 30);
//         ++itr;
//         REQUIRE(itr->price == 200);
//         REQUIRE(itr->qty == 20);
//         ++itr;
//         REQUIRE(itr->price == 100);
//         REQUIRE(itr->qty == 10);
//     }

//     SECTION("asks")
//     {
//         order o1(Side::Ask, 100, 10);
//         order o2(Side::Ask, 200, 20);
//         order o3(Side::Ask, 300, 30);
//         order o4(Side::Ask, 400, 40);
//         order o5(Side::Ask, 500, 50);

//         book.add_order(o3);
//         book.add_order(o2);
//         book.add_order(o5);
//         book.add_order(o1);
//         book.add_order(o4);

//         auto const& asks = book.asks();
//         REQUIRE(asks.size() == 5);

//         auto itr = asks.cbegin();
//         REQUIRE(itr->price == 100);
//         REQUIRE(itr->qty == 10);
//         ++itr;
//         REQUIRE(itr->price == 200);
//         REQUIRE(itr->qty == 20);
//         ++itr;
//         REQUIRE(itr->price == 300);
//         REQUIRE(itr->qty == 30);
//         ++itr;
//         REQUIRE(itr->price == 400);
//         REQUIRE(itr->qty == 40);
//         ++itr;
//         REQUIRE(itr->price == 500);
//         REQUIRE(itr->qty == 50);
//     }
// }
