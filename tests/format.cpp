#include "test_base.hpp"

#include "context.hpp"
#include "cz/defer.hpp"
#include "cz/format.hpp"
#include "cz/mem/arena.hpp"
#include "mem/mock_allocate.hpp"

using namespace cz;
using namespace cz::test;

TEST_CASE("sprint works") {
    mem::AlignedBuffer<32> buffer;
    mem::Arena arena;
    arena.mem = buffer;
    auto string = format::sprint(arena.allocator(), 123, " + ", 456);

    REQUIRE(string == "123 + 456");
}
