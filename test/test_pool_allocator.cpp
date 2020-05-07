#include "pool_allocator/pool_allocator.hpp"
#include <catch2/catch.hpp>
#include <cstring>
#include <list>
#include <unordered_map>


// void*
// operator new(std::size_t size)
// {
//     std::printf("new(%zu)\n", size);
//     void* p = malloc(size);
//     return p;
// }

struct object
{
    object(int aa, int bb, int cc)
            : a(aa)
            , b(bb)
            , c(cc)
    {}
    int a, b, c;
};

TEST_CASE("pool allocator", "[pool_allocator]")
{
    SECTION("allocator requirements")
    {
        pool_allocator<int, 10> alloc1;
        pool_allocator<int, 10> alloc2;

        REQUIRE_FALSE(alloc1 != alloc2);
        REQUIRE(alloc1 == alloc2);

        pool_allocator<int, 10> alloc3(std::move(alloc1));
        pool_allocator<int, 10> alloc4 = std::move(alloc2);
    }

    SECTION("allocate and deallocate")
    {
        pool_allocator<int, 10> alloc;
        REQUIRE(alloc.capacity() == 10);
        REQUIRE(alloc.capacity_in_bytes() == sizeof(int*) * 10);
        REQUIRE(alloc.size() == 0);
        REQUIRE(alloc.max_size_reached() == 0);

        // ensure array starts with all nullptrs
        int* ptrs[10] = {};
        for (int i = 0; i < 10; ++i)
            REQUIRE(ptrs[i] == nullptr);

        // set every array element to it's index value
        for (int i = 0; i < 10; ++i) {
            ptrs[i] = alloc.allocate(1);
            *ptrs[i] = i;
        }

        // ensure array contains 0,1,2,...,9
        for (int i = 0; i < 10; ++i)
            REQUIRE(*ptrs[i] == i);

        REQUIRE(alloc.size() == 10);
        REQUIRE(alloc.max_size_reached() == 10);

        // deallocate 5th ptr and reallocate and set to 500
        alloc.deallocate(ptrs[5], 1);
        REQUIRE(alloc.size() == 9);
        int* ptr = alloc.allocate(1);
        *ptr = 500;
        REQUIRE(alloc.size() == 10);

        for (int i = 0; i < 10; ++i) {
            if (i == 5) {
                REQUIRE(*ptrs[i] == i * 100);
            } else {
                REQUIRE(*ptrs[i] == i);
            }
        }

        // deallocate 9th ptr and reallocate and set to 900
        alloc.deallocate(ptrs[9], 1);
        REQUIRE(alloc.size() == 9);
        ptr = alloc.allocate(1);
        *ptr = 900;
        REQUIRE(alloc.size() == 10);

        for (int i = 0; i < 10; ++i) {
            if (i == 5 || i == 9) {
                REQUIRE(*ptrs[i] == i * 100);
            } else {
                REQUIRE(*ptrs[i] == i);
            }
        }

        alloc.deallocate(ptrs[0], 1);
        alloc.deallocate(ptrs[2], 1);
        alloc.deallocate(ptrs[4], 1);
        REQUIRE(alloc.size() == 7);

        ptr = alloc.allocate(1);
        *ptr = 400;
        ptr = alloc.allocate(1);
        *ptr = 200;
        ptr = alloc.allocate(1);
        *ptr = 0;
        REQUIRE(alloc.size() == 10);

        for (int i = 0; i < 10; ++i) {
            if (i == 0 || i == 2 || i == 4 || i == 5 || i == 9) {
                REQUIRE(*ptrs[i] == i * 100);
            } else {
                REQUIRE(*ptrs[i] == i);
            }
        }

        // attempt to allocate when no mem left
        REQUIRE(alloc.size() == alloc.capacity());
        REQUIRE_THROWS_AS(alloc.allocate(1), std::bad_alloc);
        REQUIRE(alloc.max_size_reached() == 10);
    }

    SECTION("allocate custom object")
    {
        pool_allocator<object, 10> alloc;
        REQUIRE(alloc.capacity() == 10);
        REQUIRE(alloc.capacity_in_bytes() == 160);

        object* obj = alloc.allocate(1);
        REQUIRE(obj != nullptr);
        alloc.construct(obj, 1, 2, 3);
        REQUIRE(obj->a == 1);
        REQUIRE(obj->b == 2);
        REQUIRE(obj->c == 3);
        alloc.destroy(obj);
        alloc.deallocate(obj, 1);
        REQUIRE(alloc.max_size_reached() == 1);
    }

    SECTION("int list")
    {
        std::list<int, pool_allocator<int, 10>> intlist;
        intlist.push_back(0);
        intlist.push_back(1);
        intlist.push_back(2);
        intlist.push_back(3);
        intlist.push_back(4);
        intlist.push_back(5);
        intlist.emplace_back(6);
        intlist.emplace_back(7);
        intlist.emplace_back(8);
        intlist.emplace_back(9);

        REQUIRE_THROWS_AS(intlist.push_back(100), std::bad_alloc);

        int i = 0;
        for (auto itr : intlist) {
            REQUIRE(itr == i++);
        }
        REQUIRE(intlist.get_allocator().size() == 10);
        REQUIRE(intlist.get_allocator().max_size_reached() == 10);
    }

    SECTION("custom object in list")
    {
        REQUIRE(sizeof(object) == 12);
        std::list<object, pool_allocator<object, 10>> objlist;
        objlist.push_back(object{0, 1, 2});
        objlist.push_back(object{3, 4, 5});
        objlist.push_back(object{6, 7, 8});
        objlist.push_back(object{9, 10, 11});
        objlist.push_back(object{12, 13, 14});
        objlist.push_back(object{15, 16, 17});
        objlist.push_back(object{18, 19, 20});
        objlist.push_back(object{21, 22, 23});
        objlist.push_back(object{24, 25, 26});
        objlist.push_back(object{27, 28, 29});

        REQUIRE_THROWS_AS(objlist.push_back(object{0, 0, 0}), std::bad_alloc);

        int i = 0;
        for (auto const& itr : objlist) {
            REQUIRE(itr.a == i++);
            REQUIRE(itr.b == i++);
            REQUIRE(itr.c == i++);
        }
        REQUIRE(objlist.get_allocator().size() == 10);
        REQUIRE(objlist.get_allocator().max_size_reached() == 10);
    }

    SECTION("object in unordered_map")
    {
        // std::unordered_map<int, object, std::hash<int>, std::equal_to<int>,
        // pool_allocator<std::pair<const int, object>, 10>> objmap; std::unordered_map<int, object>
        // objmap; object o(0, 0, 0); objmap.emplace(0, o); objmap[1] = object(1, 1, 1);
        // objmap.insert(std::make_pair(0, object(0, 0, 0)));
    }
}
