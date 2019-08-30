#include <czt/test_base.hpp>

#include <cz/defer.hpp>
#include <cz/format.hpp>
#include <cz/mem/arena.hpp>
#include <czt/mock_allocate.hpp>
#include "context.hpp"

using namespace cz;
using namespace cz::test;

TEST_CASE("sprint works") {
    mem::AlignedBuffer<32> buffer;
    mem::Arena arena;
    arena.mem = buffer;
    auto string = sprint(arena.allocator(), 123, " + ", 456);

    REQUIRE(string == "123 + 456");
}
