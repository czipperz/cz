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
    DynamicArena arena({buffer, 0});
    REQUIRE(arena.allocator().alloc(1) == MemSlice{nullptr, 0});
}

TEST_CASE("Arena alloc succeeds when exactly enough space left") {
    StackArena<Arena::alignment> arena;
    REQUIRE(arena.allocator().alloc(Arena::alignment) == MemSlice{arena.buffer, Arena::alignment});
}

TEST_CASE("Arena allocates in alignment sized chunks") {
    StackArena<Arena::alignment * 2> arena;
    REQUIRE(arena.allocator().alloc(Arena::alignment / 2) ==
            MemSlice{arena.buffer, Arena::alignment});
    REQUIRE(arena.allocator().alloc(Arena::alignment / 2) ==
            MemSlice{arena.buffer + Arena::alignment, Arena::alignment});
}

TEST_CASE("Arena alloc succeeds after first failure") {
    StackArena<Arena::alignment * 3> arena;
    REQUIRE(arena.allocator().alloc(Arena::alignment) == MemSlice{arena.buffer, Arena::alignment});
    REQUIRE(arena.allocator().alloc(Arena::alignment * 3) == MemSlice{nullptr, 0});
    REQUIRE(arena.allocator().alloc(Arena::alignment * 2) ==
            MemSlice{arena.buffer + Arena::alignment, Arena::alignment * 2});
}

TEST_CASE("Arena dealloc move pointer back when most recent allocation") {
    StackArena<Arena::alignment * 2> arena;
    arena.allocator().alloc(Arena::alignment);

    auto mem = arena.allocator().alloc(Arena::alignment / 2);
    arena.allocator().dealloc(mem);

    REQUIRE(arena.allocator().alloc(Arena::alignment / 4).buffer == mem.buffer);
}

TEST_CASE("Arena realloc in place expanding to fill the chunk") {
    StackArena<Arena::alignment> arena;

    auto mem = arena.allocator().alloc(Arena::alignment / 2);
    REQUIRE(mem == MemSlice{arena.buffer, Arena::alignment});

    mem = arena.allocator().realloc(mem, Arena::alignment * 3 / 4);
    REQUIRE(mem == MemSlice{arena.buffer, Arena::alignment});

    mem = arena.allocator().realloc(mem, Arena::alignment);
    REQUIRE(mem == MemSlice{arena.buffer, Arena::alignment});
}

TEST_CASE("Arena realloc in place expanding to more chunks") {
    StackArena<Arena::alignment * 2> arena;

    auto mem = arena.allocator().alloc(Arena::alignment / 2);
    REQUIRE(mem == MemSlice{arena.buffer, Arena::alignment});

    mem = arena.allocator().realloc(mem, Arena::alignment * 2);
    REQUIRE(mem == MemSlice{arena.buffer, Arena::alignment * 2});
}

TEST_CASE("Arena realloc in place expanding to more chunks fails as no more chunks") {
    StackArena<Arena::alignment * 2> arena;
    arena.allocator().alloc(Arena::alignment);

    auto mem = arena.allocator().alloc(Arena::alignment);
    REQUIRE(mem == MemSlice{arena.buffer + Arena::alignment, Arena::alignment});

    mem = arena.allocator().realloc(mem, Arena::alignment + 1);
    REQUIRE(mem == MemSlice{nullptr, 0});

    REQUIRE(arena.offset == Arena::alignment * 2);
}

TEST_CASE("Arena realloc smaller size same chunks") {
    StackArena<Arena::alignment> arena;

    auto mem = arena.allocator().alloc(Arena::alignment);
    mem = arena.allocator().realloc(mem, Arena::alignment / 2);

    REQUIRE(mem.buffer == arena.mem.buffer);
    REQUIRE(arena.offset == Arena::alignment);
}

TEST_CASE("Arena realloc smaller size less chunks") {
    StackArena<Arena::alignment * 2> arena;

    auto mem = arena.allocator().alloc(Arena::alignment * 2);
    mem = arena.allocator().realloc(mem, Arena::alignment / 2);

    REQUIRE(mem.buffer == arena.mem.buffer);
    REQUIRE(arena.offset == Arena::alignment);
}

TEST_CASE("HeapArena constructor allocates memory") {
    alignas(Arena::alignment) char buffer[Arena::alignment];
    auto mock = mock_alloc(buffer, {Arena::alignment, Arena::alignment});

    HeapArena arena(mock.allocator(), {Arena::alignment / 2, Arena::alignment / 2});

    CHECK(arena.mem == MemSlice{buffer, Arena::alignment});
    CHECK(arena.offset == 0);
    REQUIRE(mock.called);
}

TEST_CASE("HeapArena::drop deallocates memory") {
    StackArena<Arena::alignment> stack_arena;
    HeapArena arena(stack_arena.allocator(), {Arena::alignment, Arena::alignment});

    auto mock = mock_dealloc({stack_arena.buffer, Arena::alignment});

    arena.drop(mock.allocator());

    REQUIRE(mock.called);
}
