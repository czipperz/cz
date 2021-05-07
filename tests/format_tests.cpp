#include <czt/test_base.hpp>

#include <cz/aligned_buffer.hpp>
#include <cz/arena.hpp>
#include <cz/defer.hpp>
#include <cz/format.hpp>
#include <czt/mock_allocate.hpp>

using namespace cz;
using namespace cz::test;

TEST_CASE("format works") {
    Aligned_Buffer<32> buffer;
    Arena arena;
    arena.mem = buffer;
    auto string = format(arena.allocator(), 123, " + ", 456);

    REQUIRE(string == "123 + 456");
}
