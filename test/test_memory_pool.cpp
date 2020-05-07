#include "pool_allocator/allocator_storage.hpp"
#include "pool_allocator/memory_pool.hpp"
#include "pool_allocator/reference_storage.hpp"
#include "pool_allocator/std_allocator.hpp"
#include <catch2/catch.hpp>
#include <list>


struct object
{
    int a, b, c;
};

TEST_CASE("memory_pool", "[memory_pool]")
{
    SECTION("ctor")
    {
        memory_pool pool(sizeof(int) + 16, 4096);
        std::list<int, std_allocator<int, memory_pool>> list(pool);
        list.push_back(0);
        list.push_back(1);
        list.push_back(2);
        list.emplace_back(3);

        int i = 0;
        for (auto itr : list)
            REQUIRE(itr == i++);
    }
}
