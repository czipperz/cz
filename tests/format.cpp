#include "catch.hpp"

#include "../src/defer.hpp"
#include "../src/format.hpp"
#include "../src/mem/arena.hpp"
#include "context.hpp"
#include "mem/mock_allocate.hpp"

using namespace cz;
using namespace cz::test;

TEST_CASE("sprint works") {
    mem::StackArena<32> arena;
    C c = ctxt(arena.allocator());

    auto string = format::sprint(&c, 123, " + ", 456);
    CZ_DEFER(string.drop(c.allocator));

    REQUIRE(string == "123 + 456");
}

TEST_CASE("tprint works") {
    mem::StackArena<32> arena;
    auto c = ctxt(panic_allocator());
    c.temp = arena.allocator();

    auto string = format::tprint(&c, 123, " + ", 456);

    REQUIRE(string.buffer() == arena.mem.buffer);
    REQUIRE(string == "123 + 456");
    REQUIRE(arena.offset >= string.len());
}
