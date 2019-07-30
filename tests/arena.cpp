#include "catch.hpp"

#include "../src/defer.hpp"
#include "../src/mem.hpp"
#include "mock_allocator.hpp"

using namespace cz::mem;

TEST_CASE("Arena::alloc fails when no space left") {
    char buffer[1] = {0};
    Arena arena(buffer, 0);
    REQUIRE(arena.alloc(1) == NULL);
}

TEST_CASE("Arena::alloc succeeds when exactly enough space left") {
    char buffer[8] = {0};
    Arena arena(buffer, 8);
    REQUIRE(arena.alloc(8) == buffer);
}

TEST_CASE("Arena allocates at an offset") {
    char buffer[8] = {0};
    Arena arena(buffer, 8);
    REQUIRE(arena.alloc(2) == buffer);
    REQUIRE(arena.alloc(4) == buffer + 2);
    REQUIRE(arena.alloc(2) == buffer + 6);
}

TEST_CASE("Arena::alloc succeeds after first failure") {
    char buffer[8] = {0};
    Arena arena(buffer, 8);
    REQUIRE(arena.alloc(2) == buffer);
    REQUIRE(arena.alloc(4) == buffer + 2);
    REQUIRE(arena.alloc(4) == NULL);
    REQUIRE(arena.alloc(2) == buffer + 6);
}

TEST_CASE("Arena::sized allocates memory") {
    char buffer[8];
    test::MockAllocator test = {buffer, NULL, 0, 8};
    global_allocator = test;
    CZ_DEFER(global_allocator = heap::allocator());

    auto arena = Arena::sized(8);
    REQUIRE(arena.alloc(8) == buffer);

    REQUIRE(test.called);
}

TEST_CASE("Arena::drop deallocates memory") {
    char buffer[8];
    Arena arena(buffer, 8);
    test::MockAllocator test = {NULL, buffer, 8, 0};
    global_allocator = test;
    CZ_DEFER(global_allocator = heap::allocator());

    arena.drop();

    REQUIRE(test.called);
}

TEST_CASE("Arena::allocator works") {
    char buffer[8];
    Arena arena(buffer, 8);

    auto allocator = arena.allocator();

    REQUIRE(allocator.alloc(6) == buffer);
    REQUIRE(allocator.alloc(4) == NULL);
}
