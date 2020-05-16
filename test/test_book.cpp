#include "itch_parser/book.hpp"
#include <catch2/catch.hpp>
#include <cstring>

TEST_CASE("tsbook", "[book]")
{
    tsbook book;

    SECTION("add_order")
    {
        order o1{Side::Bid, 100, 100, nullptr};
        order o2{Side::Bid, 200, 200, nullptr};
        order o3{Side::Bid, 300, 300, nullptr};

        book.add_order(o1);
        REQUIRE(book.best_bid().price == o1.price);
        REQUIRE(book.best_bid().qty == o1.qty);

        book.add_order(o2);
        REQUIRE(book.best_bid().price == o2.price);
        REQUIRE(book.best_bid().qty == o2.qty);

        book.add_order(o3);
        REQUIRE(book.best_bid().price == o3.price);
        REQUIRE(book.best_bid().qty == o3.qty);

        REQUIRE(book.bids().size() == 3);
    }

    SECTION("delete_order")
    {
        order o1{Side::Bid, 100, 100, nullptr};
        order o2{Side::Bid, 200, 200, nullptr};
        order o3{Side::Bid, 300, 300, nullptr};

        book.add_order(o1);
        book.add_order(o2);
        book.add_order(o3);

        book.delete_order(o2);
        REQUIRE(book.bids().size() == 2);
        REQUIRE(o2.price == 0);
        REQUIRE(o2.qty == 0);

        // non-existent order
        order tmp{Side::Bid, 5000, 5000, nullptr};
        book.delete_order(tmp);
        REQUIRE(book.bids().size() == 2);
        REQUIRE(tmp.price == 5000);
        REQUIRE(tmp.qty == 5000);
        REQUIRE(tmp.pl == nullptr);

        book.delete_order(o3);
        REQUIRE(book.bids().size() == 1);
        REQUIRE(o3.price == 0);
        REQUIRE(o3.qty == 0);

        book.delete_order(o1);
        REQUIRE(book.bids().empty());
        REQUIRE(o1.price == 0);
        REQUIRE(o1.qty == 0);
    }

    SECTION("bids")
    {
        order o1{Side::Bid, 100, 10, nullptr};
        order o2{Side::Bid, 200, 20, nullptr};
        order o3{Side::Bid, 300, 30, nullptr};
        order o4{Side::Bid, 400, 40, nullptr};
        order o5{Side::Bid, 500, 50, nullptr};

        book.add_order(o3);
        book.add_order(o2);
        book.add_order(o5);
        book.add_order(o1);
        book.add_order(o4);

        auto const& bids = book.bids();
        REQUIRE(bids.size() == 5);

        auto itr = bids.cbegin();
        REQUIRE(itr->price == 500);
        REQUIRE(itr->qty == 50);
        ++itr;
        REQUIRE(itr->price == 400);
        REQUIRE(itr->qty == 40);
        ++itr;
        REQUIRE(itr->price == 300);
        REQUIRE(itr->qty == 30);
        ++itr;
        REQUIRE(itr->price == 200);
        REQUIRE(itr->qty == 20);
        ++itr;
        REQUIRE(itr->price == 100);
        REQUIRE(itr->qty == 10);
    }

    SECTION("asks")
    {
        order o1{Side::Ask, 100, 10, nullptr};
        order o2{Side::Ask, 200, 20, nullptr};
        order o3{Side::Ask, 300, 30, nullptr};
        order o4{Side::Ask, 400, 40, nullptr};
        order o5{Side::Ask, 500, 50, nullptr};

        book.add_order(o3);
        book.add_order(o2);
        book.add_order(o5);
        book.add_order(o1);
        book.add_order(o4);

        auto const& asks = book.asks();
        REQUIRE(asks.size() == 5);

        auto itr = asks.cbegin();
        REQUIRE(itr->price == 100);
        REQUIRE(itr->qty == 10);
        ++itr;
        REQUIRE(itr->price == 200);
        REQUIRE(itr->qty == 20);
        ++itr;
        REQUIRE(itr->price == 300);
        REQUIRE(itr->qty == 30);
        ++itr;
        REQUIRE(itr->price == 400);
        REQUIRE(itr->qty == 40);
        ++itr;
        REQUIRE(itr->price == 500);
        REQUIRE(itr->qty == 50);
    }
}
