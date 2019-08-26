#include <czt/test_base.hpp>

#include <string.h>
#include <cz/mem.hpp>
#include <czt/mock_allocate.hpp>
#include "../context.hpp"

using namespace cz;
using namespace cz::test;
using namespace cz::mem;

TEST_CASE("Arena alloc returns null when no space left") {
    char buffer[1] = {0};
    Arena arena({buffer, 0});
    REQUIRE(arena.allocator().alloc(1) == MemSlice{nullptr, 0});
}

TEST_CASE("Arena alloc succeeds when exactly enough space left") {
    AlignedBuffer<Arena::alignment> buffer;
    Arena arena;
    arena.mem = buffer;
    REQUIRE(arena.allocator().alloc(Arena::alignment) == MemSlice{buffer.buffer, Arena::alignment});
}

TEST_CASE("Arena allocates in alignment sized chunks") {
    AlignedBuffer<Arena::alignment * 2> buffer;
    Arena arena;
    arena.mem = buffer;
    REQUIRE(arena.allocator().alloc(Arena::alignment / 2) ==
            MemSlice{buffer.buffer, Arena::alignment});
    REQUIRE(arena.allocator().alloc(Arena::alignment / 2) ==
            MemSlice{buffer.buffer + Arena::alignment, Arena::alignment});
}

TEST_CASE("Arena alloc succeeds after first failure") {
    AlignedBuffer<Arena::alignment * 3> buffer;
    Arena arena;
    arena.mem = buffer;
    REQUIRE(arena.allocator().alloc(Arena::alignment) == MemSlice{buffer.buffer, Arena::alignment});
    REQUIRE(arena.allocator().alloc(Arena::alignment * 3) == MemSlice{nullptr, 0});
    REQUIRE(arena.allocator().alloc(Arena::alignment * 2) ==
            MemSlice{buffer.buffer + Arena::alignment, Arena::alignment * 2});
}

TEST_CASE("Arena dealloc move pointer back when most recent allocation") {
    AlignedBuffer<Arena::alignment * 2> buffer;
    Arena arena;
    arena.mem = buffer;
    arena.allocator().alloc(Arena::alignment);

    auto mem = arena.allocator().alloc(Arena::alignment / 2);
    arena.allocator().dealloc(mem);

    REQUIRE(arena.allocator().alloc(Arena::alignment / 4).buffer == mem.buffer);
}

TEST_CASE("Arena realloc in place expanding to fill the chunk") {
    AlignedBuffer<Arena::alignment> buffer;
    Arena arena;
    arena.mem = buffer;

    auto mem = arena.allocator().alloc(Arena::alignment / 2);
    REQUIRE(mem == MemSlice{buffer.buffer, Arena::alignment});

    mem = arena.allocator().realloc(mem, Arena::alignment * 3 / 4);
    REQUIRE(mem == MemSlice{buffer.buffer, Arena::alignment});

    mem = arena.allocator().realloc(mem, Arena::alignment);
    REQUIRE(mem == MemSlice{buffer.buffer, Arena::alignment});
}

TEST_CASE("Arena realloc in place expanding to more chunks") {
    AlignedBuffer<Arena::alignment * 2> buffer;
    Arena arena;
    arena.mem = buffer;

    auto mem = arena.allocator().alloc(Arena::alignment / 2);
    REQUIRE(mem == MemSlice{buffer.buffer, Arena::alignment});

    mem = arena.allocator().realloc(mem, Arena::alignment * 2);
    REQUIRE(mem == MemSlice{buffer.buffer, Arena::alignment * 2});
}

TEST_CASE("Arena realloc in place expanding to more chunks fails as no more chunks") {
    AlignedBuffer<Arena::alignment * 2> buffer;
    Arena arena;
    arena.mem = buffer;
    arena.allocator().alloc(Arena::alignment);

    auto mem = arena.allocator().alloc(Arena::alignment);
    REQUIRE(mem == MemSlice{buffer.buffer + Arena::alignment, Arena::alignment});

    mem = arena.allocator().realloc(mem, Arena::alignment + 1);
    REQUIRE(mem == MemSlice{nullptr, 0});

    REQUIRE(arena.offset == Arena::alignment * 2);
}

TEST_CASE("Arena realloc smaller size same chunks") {
    AlignedBuffer<Arena::alignment> buffer;
    Arena arena;
    arena.mem = buffer;

    auto mem = arena.allocator().alloc(Arena::alignment);
    mem = arena.allocator().realloc(mem, Arena::alignment / 2);

    REQUIRE(mem.buffer == arena.mem.buffer);
    REQUIRE(arena.offset == Arena::alignment);
}

TEST_CASE("Arena realloc smaller size less chunks") {
    AlignedBuffer<Arena::alignment * 2> buffer;
    Arena arena;
    arena.mem = buffer;

    auto mem = arena.allocator().alloc(Arena::alignment * 2);
    mem = arena.allocator().realloc(mem, Arena::alignment / 2);

    REQUIRE(mem.buffer == arena.mem.buffer);
    REQUIRE(arena.offset == Arena::alignment);
}
