#include "pool_allocator/free_list.hpp"
#include <catch2/catch.hpp>


struct object
{
    int a, b, c;
};

TEST_CASE("free_list", "[free_list]")
{
    REQUIRE(sizeof(object) == 12);

    SECTION("alloc-dealloc")
    {
        void* mem = std::malloc(sizeof(object) * 5);

        free_list fl(sizeof(object), mem, sizeof(object) * 5);
        REQUIRE(fl.capacity() == 5);

        // allocate 5 objects and set values
        object* objs[5] = {};
        for (int i = 0; i < 5; ++i) {
            objs[i] = static_cast<object*>(fl.allocate());
            REQUIRE(objs[i] != nullptr);
            objs[i]->a = objs[i]->b = objs[i]->c = i;
            REQUIRE(fl.capacity() == static_cast<std::size_t>(5 - i - 1));
        }
        REQUIRE(fl.capacity() == 0);
        REQUIRE(fl.allocate() == nullptr);

        // verify object values
        for (int i = 0; i < 5; ++i) {
            REQUIRE(objs[i]->a == i);
            REQUIRE(objs[i]->b == i);
            REQUIRE(objs[i]->c == i);
        }

        // deallocate 2 objs (out of order)
        fl.deallocate(objs[3]);
        REQUIRE(fl.capacity() == 1);
        fl.deallocate(objs[1]);
        REQUIRE(fl.capacity() == 2);

        objs[3] = static_cast<object*>(fl.allocate());
        objs[3]->a = objs[3]->b = objs[3]->c = 300;
        REQUIRE(fl.capacity() == 1);

        objs[1] = static_cast<object*>(fl.allocate());
        objs[1]->a = objs[1]->b = objs[1]->c = 100;
        REQUIRE(fl.capacity() == 0);

        // verify object values
        for (int i = 0; i < 5; ++i) {
            REQUIRE(objs[i]->a == ((i == 1 || i == 3) ? i * 100 : i));
            REQUIRE(objs[i]->b == ((i == 1 || i == 3) ? i * 100 : i));
            REQUIRE(objs[i]->c == ((i == 1 || i == 3) ? i * 100 : i));
        }

        // deallocate everything
        for (std::size_t i = 0; i < 5; ++i) {
            REQUIRE(fl.capacity() == i);
            fl.deallocate(objs[i]);
        }

        std::free(mem);
    }
}
