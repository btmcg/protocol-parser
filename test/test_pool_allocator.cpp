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
    // pool_allocator<int> alloc;

    // SECTION("basic")
    // {
    //     int* ptr = alloc.allocate(sizeof(int));
    //     *ptr = 42;
    //     REQUIRE(*ptr == 42);

    //     alloc.deallocate(ptr, sizeof(int));
    // }

    SECTION("replacement for std::allocator")
    {
        std::list<object, pool_allocator<object>> objlist;
        objlist.push_back(object{1, 2, 3});
        objlist.push_back(object{4, 5, 6});
        objlist.push_back(object{7, 8, 9});

        std::list<int, pool_allocator<int>> intlist;
        intlist.push_back(42);
        intlist.push_back(43);
        intlist.push_back(44);
        intlist.push_back(45);
    }
}
