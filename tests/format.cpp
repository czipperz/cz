#include "catch.hpp"

#include "../src/defer.hpp"
#include "../src/format.hpp"
#include "../src/mem/arena.hpp"
#include "context.hpp"

using namespace cz;

TEST_CASE("sprint works") {
    mem::StackArena<32> arena;
    C c = test::ctxt(arena.allocator());

    auto string = format::sprint(&c, 123, " + ", 456);
    CZ_DEFER(string.drop(&c));

    REQUIRE(string == "123 + 456");
}
