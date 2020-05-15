#include "itch_parser/book.hpp"
#include <catch2/catch.hpp>
#include <cstring>

TEST_CASE("tsbook", "[book]")
{
    tsbook book;

    SECTION("add_order")
    {
        {
            book.add_order(1, Side::Bid, 100, 100);
            auto const& orders = book.order_list();
            REQUIRE(orders.contains(1));
            order const& o = orders.find(1)->second;
            REQUIRE(o.price == 100);
            REQUIRE(o.qty == 100);
        }

        {
            book.add_order(2, Side::Bid, 200, 200);
            auto const& orders = book.order_list();
            REQUIRE(orders.contains(2));
            order const& o = orders.find(2)->second;
            REQUIRE(o.price == 200);
            REQUIRE(o.qty == 200);
        }

        {
            book.add_order(3, Side::Bid, 300, 300);
            auto const& orders = book.order_list();
            REQUIRE(orders.contains(3));
            order const& o = orders.find(3)->second;
            REQUIRE(o.price == 300);
            REQUIRE(o.qty == 300);
        }
    }

    SECTION("delete_order")
    {
        book.add_order(1, Side::Bid, 100, 100);
        book.add_order(2, Side::Bid, 200, 200);
        book.add_order(3, Side::Bid, 300, 300);
        REQUIRE(book.order_list().size() == 3);

        book.delete_order(2);
        REQUIRE_FALSE(book.order_list().contains(2));
        REQUIRE(book.order_list().size() == 2);

        // non-existent order
        book.delete_order(5);
        REQUIRE(book.order_list().size() == 2);

        book.delete_order(3);
        REQUIRE_FALSE(book.order_list().contains(3));
        REQUIRE(book.order_list().size() == 1);

        book.delete_order(1);
        REQUIRE_FALSE(book.order_list().contains(1));
        REQUIRE(book.order_list().size() == 0);
        REQUIRE(book.order_list().empty());
    }

    SECTION("bids")
    {
        book.add_order(3, Side::Bid, 300, 30);
        book.add_order(2, Side::Bid, 200, 20);
        book.add_order(5, Side::Bid, 500, 50);
        book.add_order(1, Side::Bid, 100, 10);
        book.add_order(4, Side::Bid, 400, 40);

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
        book.add_order(3, Side::Ask, 300, 30);
        book.add_order(2, Side::Ask, 200, 20);
        book.add_order(5, Side::Ask, 500, 50);
        book.add_order(1, Side::Ask, 100, 10);
        book.add_order(4, Side::Ask, 400, 40);

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
