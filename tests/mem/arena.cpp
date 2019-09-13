#include <czt/test_base.hpp>

#include <string.h>
#include <cz/arena.hpp>
#include <czt/mock_allocate.hpp>
#include "../context.hpp"

using namespace cz;
using namespace cz::test;

TEST_CASE("Arena alloc returns null when no space left") {
    char buffer[1] = {0};
    Arena arena({buffer, 0});
    REQUIRE(arena.allocator().alloc({1, 1}) == MemSlice{nullptr, 0});
}

TEST_CASE("Arena alloc succeeds when exactly enough space left") {
    char buffer[8] = {0};
    Arena arena(buffer);
    REQUIRE(arena.allocator().alloc({8, 1}) == MemSlice{buffer, 8});
}

TEST_CASE("Arena allocates at an offset") {
    char buffer[8] = {0};
    Arena arena(buffer);
    REQUIRE(arena.allocator().alloc({2, 1}) == MemSlice{buffer, 2});
    REQUIRE(arena.allocator().alloc({4, 1}) == MemSlice{buffer + 2, 4});
    REQUIRE(arena.allocator().alloc({2, 1}) == MemSlice{buffer + 6, 2});
}

TEST_CASE("Arena alloc succeeds after first failure") {
    char buffer[8] = {0};
    Arena arena(buffer);
    REQUIRE(arena.allocator().alloc({2, 1}) == MemSlice{buffer, 2});
    REQUIRE(arena.allocator().alloc({4, 1}) == MemSlice{buffer + 2, 4});
    REQUIRE(arena.allocator().alloc({4, 1}) == MemSlice{nullptr, 0});
    REQUIRE(arena.allocator().alloc({2, 1}) == MemSlice{buffer + 6, 2});
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
    arena.allocator().alloc({2, 1});

    auto mem = arena.allocator().alloc({2, 1});
    arena.allocator().dealloc(mem);

    REQUIRE(arena.allocator().alloc({3, 1}).buffer == mem.buffer);
}

TEST_CASE("Arena realloc in place expanding") {
    char buffer[8] = {0};
    Arena arena(buffer);

    auto mem = arena.allocator().alloc({4, 1});
    REQUIRE(mem == MemSlice{buffer, 4});
    memset(mem.buffer, '*', 4);

    mem = arena.allocator().realloc(mem, {6, 1});
    REQUIRE(mem == MemSlice{buffer, 6});
    REQUIRE(buffer[0] == '*');
    REQUIRE(buffer[1] == '*');
    REQUIRE(buffer[2] == '*');
    REQUIRE(buffer[3] == '*');
}

TEST_CASE("Arena realloc in place expanding failure boundary success") {
    char buffer[8] = {0};
    Arena arena(buffer);
    arena.allocator().alloc({2, 1});

    auto mem = arena.allocator().alloc({4, 1});
    REQUIRE(mem == MemSlice{buffer + 2, 4});

    mem = arena.allocator().realloc(mem, {6, 1});
    REQUIRE(mem == MemSlice{buffer + 2, 6});
}

TEST_CASE("Arena realloc in place expanding failure boundary error") {
    char buffer[8] = {0};
    Arena arena(buffer);
    arena.allocator().alloc({2, 1});

    auto mem = arena.allocator().alloc({4, 1});
    REQUIRE(mem == MemSlice{buffer + 2, 4});

    mem = arena.allocator().realloc(mem, {7, 1});
    REQUIRE(mem == MemSlice{nullptr, 0});
}

TEST_CASE("Arena realloc not enough space returns null") {
    char buffer[8] = {0};
    Arena arena(buffer);

    auto mem = arena.allocator().alloc({4, 1});
    REQUIRE(arena.allocator().realloc(mem, {10, 1}).buffer == nullptr);

    REQUIRE(arena.offset == 4);
}

TEST_CASE("Arena realloc smaller size") {
    char buffer[8] = {0};
    Arena arena(buffer);

    auto mem = arena.allocator().alloc({4, 1});
    mem = arena.allocator().realloc(mem, {2, 1});

    REQUIRE(mem.buffer == arena.mem.buffer);
    REQUIRE(arena.offset == 2);
}
