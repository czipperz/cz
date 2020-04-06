#include <czt/test_base.hpp>

#include <string.h>
#include <cz/arena.hpp>
#include <czt/mock_allocate.hpp>

using namespace cz;
using namespace cz::test;

TEST_CASE("Arena alloc returns null when no space left") {
    char buffer[1] = {0};
    Arena arena({buffer, 0});
    REQUIRE(arena.allocator().alloc({1, 1}) == nullptr);
}

TEST_CASE("Arena alloc succeeds when exactly enough space left") {
    char buffer[8] = {0};
    Arena arena(buffer);
    REQUIRE(arena.allocator().alloc({8, 1}) == buffer);
}

TEST_CASE("Arena allocates at an offset") {
    char buffer[8] = {0};
    Arena arena(buffer);
    REQUIRE(arena.allocator().alloc({2, 1}) == buffer);
    REQUIRE(arena.allocator().alloc({4, 1}) == buffer + 2);
    REQUIRE(arena.allocator().alloc({2, 1}) == buffer + 6);
}

TEST_CASE("Arena alloc succeeds after first failure") {
    char buffer[8] = {0};
    Arena arena(buffer);
    REQUIRE(arena.allocator().alloc({2, 1}) == buffer);
    REQUIRE(arena.allocator().alloc({4, 1}) == buffer + 2);
    REQUIRE(arena.allocator().alloc({4, 1}) == nullptr);
    REQUIRE(arena.allocator().alloc({2, 1}) == buffer + 6);
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
    arena.allocator().alloc({2, 1});

    auto mem = arena.allocator().alloc({2, 1});
    arena.allocator().dealloc({mem, 2});

    REQUIRE(arena.allocator().alloc({3, 1}) == mem);
}

TEST_CASE("Arena realloc in place expanding") {
    char buffer[8] = {0};
    Arena arena(buffer);

    auto mem = arena.allocator().alloc({4, 1});
    REQUIRE(mem == buffer);
    memset(mem, '*', 4);

    mem = arena.allocator().realloc({mem, 4}, {6, 1});
    REQUIRE(mem == buffer);
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
    REQUIRE(mem == buffer + 2);

    mem = arena.allocator().realloc({mem, 4}, {6, 1});
    REQUIRE(mem == buffer + 2);
}

TEST_CASE("Arena realloc in place expanding failure boundary error") {
    char buffer[8] = {0};
    Arena arena(buffer);
    arena.allocator().alloc({2, 1});

    auto mem = arena.allocator().alloc({4, 1});
    REQUIRE(mem == buffer + 2);

    mem = arena.allocator().realloc({mem, 4}, {7, 1});
    REQUIRE(mem == nullptr);
}

TEST_CASE("Arena realloc not enough space returns null") {
    char buffer[8] = {0};
    Arena arena(buffer);

    auto mem = arena.allocator().alloc({4, 1});
    REQUIRE(arena.allocator().realloc({mem, 4}, {10, 1}) == nullptr);

    REQUIRE(arena.offset == 4);
}

TEST_CASE("Arena realloc smaller size") {
    char buffer[8] = {0};
    Arena arena(buffer);

    auto mem = arena.allocator().alloc({4, 1});
    mem = arena.allocator().realloc({mem, 4}, {2, 1});

    REQUIRE(mem == arena.mem.buffer);
    REQUIRE(arena.offset == 2);
}
