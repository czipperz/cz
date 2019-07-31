#include "catch.hpp"

#include "../src/mem.hpp"
#include "mock_allocator.hpp"

using namespace cz::mem;

TEST_CASE("Arena::alloc fails when no space left") {
    char buffer[1] = {0};
    Arena arena(buffer, 0);
    REQUIRE(arena.allocator().alloc({1, 1}) == NULL);
}

TEST_CASE("Arena::alloc succeeds when exactly enough space left") {
    char buffer[8] = {0};
    Arena arena(buffer, 8);
    REQUIRE(arena.allocator().alloc({8, 1}) == buffer);
}

TEST_CASE("Arena allocates at an offset") {
    char buffer[8] = {0};
    Arena arena(buffer, 8);
    REQUIRE(arena.allocator().alloc({2, 1}) == buffer);
    REQUIRE(arena.allocator().alloc({4, 1}) == buffer + 2);
    REQUIRE(arena.allocator().alloc({2, 1}) == buffer + 6);
}

TEST_CASE("Arena::alloc succeeds after first failure") {
    char buffer[8] = {0};
    Arena arena(buffer, 8);
    REQUIRE(arena.allocator().alloc({2, 1}) == buffer);
    REQUIRE(arena.allocator().alloc({4, 1}) == buffer + 2);
    REQUIRE(arena.allocator().alloc({4, 1}) == NULL);
    REQUIRE(arena.allocator().alloc({2, 1}) == buffer + 6);
}

TEST_CASE("Arena::drop deallocates memory") {
    char buffer[8];
    Arena arena(buffer, 8);
    test::MockAllocator test = {NULL, {buffer, 8}, {0, 0}};

    with_global_allocator(test, [&]() {
        arena.drop();

        REQUIRE(test.called);
    });
}

TEST_CASE("Arena::allocator works") {
    char buffer[8];
    Arena arena(buffer, 8);

    auto allocator = arena.allocator();

    REQUIRE(allocator.alloc({6, 1}) == buffer);
    REQUIRE(allocator.alloc({4, 1}) == NULL);
}
