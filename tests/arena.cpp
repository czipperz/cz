#include "catch.hpp"

#include "../src/mem.hpp"

using cz::mem::Arena;
using cz::mem::global_allocator;

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

struct TestRealloc {
    void* buffer;
    void* expected_old_ptr;
    size_t expected_old_size;
    size_t expected_new_size;
    bool called;
};

void* test_realloc(void* _data,
                   void* old_ptr,
                   size_t old_size,
                   size_t new_size) {
    TestRealloc* data = static_cast<TestRealloc*>(_data);
    REQUIRE(data->expected_old_ptr == old_ptr);
    REQUIRE(data->expected_old_size == old_size);
    REQUIRE(data->expected_new_size == new_size);
    data->called = true;
    return data->buffer;
}

TEST_CASE("Arena::sized allocates memory") {
    char buffer[8];
    TestRealloc test = {buffer, NULL, 0, 8, false};
    global_allocator = {test_realloc, &test};

    auto arena = Arena::sized(8);
    REQUIRE(arena.alloc(8) == buffer);

    REQUIRE(test.called);
}

TEST_CASE("Arena::drop deallocates memory") {
    char buffer[8];
    Arena arena(buffer, 8);
    TestRealloc test = {NULL, buffer, 8, 0, false};
    global_allocator = {test_realloc, &test};

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
