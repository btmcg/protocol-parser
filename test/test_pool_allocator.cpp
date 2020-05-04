#include "pool_allocator/pool_allocator.hpp"
#include <catch2/catch.hpp>
#include <cstring>
#include <list>


// void*
// operator new(std::size_t size)
// {
//     std::printf("new(%zu)\n", size);
//     void* p = malloc(size);
//     return p;
// }

struct object
{
    int a, b, c;
};

TEST_CASE("pool allocator", "[pool_allocator]")
{
    SECTION("allocator requirements")
    {
        pool_allocator<int> alloc1;
        pool_allocator<int> alloc2;

        REQUIRE(alloc1 != alloc2);
        REQUIRE_FALSE(alloc1 == alloc2);

        // pool_allocator<int> alloc3(alloc1);
        // pool_allocator<int> alloc4 = alloc1;
        // pool_allocator<int> alloc5(std::move(alloc1));
        // pool_allocator<int> alloc6 = std::move(alloc1);
    }

    SECTION("allocate and deallocate")
    {
        pool_allocator<int> alloc;
        REQUIRE(alloc.capacity() == sizeof(int*) * 10);

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

        // deallocate 5th ptr and reallocate and set to 500
        alloc.deallocate(ptrs[5], 1);
        int* ptr = alloc.allocate(1);
        *ptr = 500;

        for (int i = 0; i < 10; ++i) {
            if (i == 5) {
                REQUIRE(*ptrs[i] == i * 100);
            } else {
                REQUIRE(*ptrs[i] == i);
            }
        }

        // deallocate 9th ptr and reallocate and set to 900
        alloc.deallocate(ptrs[9], 1);
        ptr = alloc.allocate(1);
        *ptr = 900;

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

        ptr = alloc.allocate(1);
        *ptr = 400;
        ptr = alloc.allocate(1);
        *ptr = 200;
        ptr = alloc.allocate(1);
        *ptr = 0;

        for (int i = 0; i < 10; ++i) {
            if (i == 0 || i == 2 || i == 4 || i == 5 || i == 9) {
                REQUIRE(*ptrs[i] == i * 100);
            } else {
                REQUIRE(*ptrs[i] == i);
            }
        }

        // attempt to allocate when no mem left
        REQUIRE(alloc.allocate(1) == nullptr);
    }

    // SECTION("int list")
    // {
    //     std::list<int, pool_allocator<int, 10>> intlist;
    //     intlist.push_back(42);
    //     intlist.push_back(43);
    //     intlist.push_back(44);
    //     intlist.push_back(45);
    // }

    // SECTION("custom object in list")
    // {
    //     REQUIRE(sizeof(object) == 12);
    //     std::list<object, pool_allocator<object, 10>> objlist;
    //     objlist.push_back(object{1, 2, 3});
    //     objlist.push_back(object{4, 5, 6});
    //     objlist.push_back(object{7, 8, 9});
    // }
}
