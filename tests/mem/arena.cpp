#include "../catch.hpp"

#include <string.h>
#include "../../src/mem.hpp"
#include "../context.hpp"
#include "mock_allocate.hpp"

using namespace cz;
using namespace cz::test;
using namespace cz::mem;

TEST_CASE("Arena alloc returns null when no space left") {
    char buffer[1] = {0};
    Arena arena({buffer, 0});
    REQUIRE(arena.allocator().alloc(1) == MemSlice{NULL, 0});
}

TEST_CASE("Arena alloc succeeds when exactly enough space left") {
    char buffer[8] = {0};
    Arena arena(buffer);
    REQUIRE(arena.allocator().alloc(8) == MemSlice{buffer, 8});
}

TEST_CASE("Arena allocates at an offset") {
    char buffer[8] = {0};
    Arena arena(buffer);
    REQUIRE(arena.allocator().alloc(2) == MemSlice{buffer, 2});
    REQUIRE(arena.allocator().alloc(4) == MemSlice{buffer + 2, 4});
    REQUIRE(arena.allocator().alloc(2) == MemSlice{buffer + 6, 2});
}

TEST_CASE("Arena alloc succeeds after first failure") {
    char buffer[8] = {0};
    Arena arena(buffer);
    REQUIRE(arena.allocator().alloc(2) == MemSlice{buffer, 2});
    REQUIRE(arena.allocator().alloc(4) == MemSlice{buffer + 2, 4});
    REQUIRE(arena.allocator().alloc(4) == MemSlice{NULL, 0});
    REQUIRE(arena.allocator().alloc(2) == MemSlice{buffer + 6, 2});
}

TEST_CASE() {
    char buffer[8] = {0};
    Arena arena(buffer);

    auto start = arena.allocator().alloc({1, 2}).buffer;
    REQUIRE(start);
    auto offset = arena.allocator().alloc({1, 2}).buffer;
    REQUIRE(offset == (void*)((char*)start + 2));
}

TEST_CASE("Arena dealloc move pointer back when most recent allocation") {
    char buffer[8] = {0};
    Arena arena(buffer);
    arena.allocator().alloc(2);

    auto mem = arena.allocator().alloc(2);
    arena.allocator().dealloc(mem);

    REQUIRE(arena.allocator().alloc(3).buffer == mem.buffer);
}

TEST_CASE("Arena realloc in place expanding") {
    char buffer[8] = {0};
    Arena arena(buffer);

    auto mem = arena.allocator().alloc(4);
    REQUIRE(mem == MemSlice{buffer, 4});
    memset(mem.buffer, '*', 4);

    mem = arena.allocator().realloc(mem, 6);
    REQUIRE(mem == MemSlice{buffer, 6});
    REQUIRE(buffer[0] == '*');
    REQUIRE(buffer[1] == '*');
    REQUIRE(buffer[2] == '*');
    REQUIRE(buffer[3] == '*');
}

TEST_CASE("Arena realloc in place expanding failure boundary success") {
    char buffer[8] = {0};
    Arena arena(buffer);
    arena.allocator().alloc(2);

    auto mem = arena.allocator().alloc(4);
    REQUIRE(mem == MemSlice{buffer + 2, 4});

    mem = arena.allocator().realloc(mem, 6);
    REQUIRE(mem == MemSlice{buffer + 2, 6});
}

TEST_CASE("Arena realloc in place expanding failure boundary error") {
    char buffer[8] = {0};
    Arena arena(buffer);
    arena.allocator().alloc(2);

    auto mem = arena.allocator().alloc(4);
    REQUIRE(mem == MemSlice{buffer + 2, 4});

    mem = arena.allocator().realloc(mem, 7);
    REQUIRE(mem == MemSlice{NULL, 0});
}

TEST_CASE("Arena realloc not enough space returns null") {
    char buffer[8] = {0};
    Arena arena(buffer);

    auto mem = arena.allocator().alloc(4);
    REQUIRE(arena.allocator().realloc(mem, 10).buffer == NULL);

    REQUIRE(arena.offset == 4);
}

TEST_CASE("Arena realloc smaller size") {
    char buffer[8] = {0};
    Arena arena(buffer);

    auto mem = arena.allocator().alloc(4);
    mem = arena.allocator().realloc(mem, 2);

    REQUIRE(mem.buffer == arena.mem.buffer);
    REQUIRE(arena.offset == 2);
}

TEST_CASE("HeapArena constructor allocates memory") {
    char buffer[8];
    auto mock = mock_alloc(buffer, {4, 2});

    HeapArena arena(mock.allocator(), {4, 2});

    CHECK(arena.mem.buffer == buffer);
    CHECK(arena.mem.size == 4);
    CHECK(arena.offset == 0);
    REQUIRE(mock.called);
}

TEST_CASE("HeapArena::drop deallocates memory") {
    StackArena<8> stack_arena;
    HeapArena arena(stack_arena.allocator(), {4, 2});

    auto mock = mock_dealloc({stack_arena.buffer, 4});

    arena.drop(mock.allocator());

    REQUIRE(mock.called);
}
