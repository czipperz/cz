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
    auto string = format::sprint(arena.allocator(), 123, " + ", 456);

    REQUIRE(string == "123 + 456");
}
