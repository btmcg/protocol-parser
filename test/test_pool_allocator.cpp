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

TEST_CASE("pool allocator", "[pool_allocator]")
{
    pool_allocator<int> alloc;

    SECTION("basic")
    {
        int* ptr = alloc.allocate(1);
        *ptr = 42;
        REQUIRE(*ptr == 42);

        alloc.deallocate(ptr, 1);
    }

    SECTION("replacement for std::allocator")
    {
        // std::printf("start\n");
        std::list<int, pool_allocator<int>> mylist;
        // std::list<int> mylist;
        mylist.push_back(42);
        mylist.push_back(43);
        mylist.push_back(44);
        mylist.push_back(45);
        mylist.push_back(46);
        // std::printf("end\n");
    }
}
