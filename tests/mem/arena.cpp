#include "../catch.hpp"

#include <string.h>
#include "../../src/mem.hpp"
#include "mock_allocate.hpp"

using namespace cz::mem;

TEST_CASE("Arena alloc returns null when no space left") {
    char buffer[1] = {0};
    Arena arena({buffer, 0});
    REQUIRE(arena.allocator().alloc(1) == NULL);
}

TEST_CASE("Arena alloc succeeds when exactly enough space left") {
    char buffer[8] = {0};
    Arena arena(buffer);
    REQUIRE(arena.allocator().alloc(8) == buffer);
}

TEST_CASE("Arena allocates at an offset") {
    char buffer[8] = {0};
    Arena arena(buffer);
    REQUIRE(arena.allocator().alloc(2) == buffer);
    REQUIRE(arena.allocator().alloc(4) == buffer + 2);
    REQUIRE(arena.allocator().alloc(2) == buffer + 6);
}

TEST_CASE("Arena alloc succeeds after first failure") {
    char buffer[8] = {0};
    Arena arena(buffer);
    REQUIRE(arena.allocator().alloc(2) == buffer);
    REQUIRE(arena.allocator().alloc(4) == buffer + 2);
    REQUIRE(arena.allocator().alloc(4) == NULL);
    REQUIRE(arena.allocator().alloc(2) == buffer + 6);
}

TEST_CASE() {
    char buffer[8] = {0};
    Arena arena(buffer);

    auto start = arena.allocator().alloc({1, 2});
    REQUIRE(start);
    auto offset = arena.allocator().alloc({1, 2});
    REQUIRE(offset == (void*)((char*)start + 2));
}

TEST_CASE("Arena dealloc move pointer back when most recent allocation") {
    char buffer[8] = {0};
    Arena arena(buffer);
    arena.allocator().alloc(2);
    void* ptr = arena.allocator().alloc(2);

    arena.allocator().dealloc({ptr, 2});

    REQUIRE(arena.allocator().alloc(3) == ptr);
}

TEST_CASE("Arena realloc in place expanding") {
    char buffer[8] = {0};
    Arena arena(buffer);

    auto buf = arena.allocator().alloc(4);
    REQUIRE(buf == buffer);
    memset(buf, '*', 4);

    buf = arena.allocator().realloc({buf, 4}, 6);
    REQUIRE(buf == buffer);
    REQUIRE(buffer[0] == '*');
    REQUIRE(buffer[1] == '*');
    REQUIRE(buffer[2] == '*');
    REQUIRE(buffer[3] == '*');
}

TEST_CASE("Arena realloc not enough space returns null") {
    char buffer[8] = {0};
    Arena arena(buffer);

    auto buf = arena.allocator().alloc(4);
    REQUIRE(arena.allocator().realloc({buf, 4}, 10) == NULL);

    REQUIRE(arena.offset == 4);
}

TEST_CASE("Arena realloc smaller size") {
    char buffer[8] = {0};
    Arena arena(buffer);

    auto buf = arena.allocator().alloc(4);
    buf = arena.allocator().realloc({buf, 4}, 2);

    REQUIRE(buf == arena.mem.buffer);
    REQUIRE(arena.offset == 2);
}

TEST_CASE("Arena::drop deallocates memory") {
    char buffer[8];
    Arena arena(buffer);
    auto mock = test::mock_dealloc(buffer);

    with_global_allocator(mock.allocator(), [&]() { arena.drop(); });

    REQUIRE(mock.called);
}
