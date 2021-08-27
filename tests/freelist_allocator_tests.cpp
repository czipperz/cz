#include <czt/test_base.hpp>

#include <cz/freelist_allocator.hpp>

TEST_CASE("freelist_heap 1") {
    cz::Freelist_Heap freelist = {};
    CZ_DEFER(freelist.drop());
    cz::Allocator allocator = freelist.allocator();

    int* i1 = allocator.alloc<int>();
    REQUIRE(i1);
    allocator.dealloc(i1);

    int* i2 = allocator.alloc<int>();
    REQUIRE(i2);
    allocator.dealloc(i2);

    CHECK(i1 == i2);
}
