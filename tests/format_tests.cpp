#include <czt/test_base.hpp>

#include <cz/aligned_buffer.hpp>
#include <cz/arena.hpp>
#include <cz/defer.hpp>
#include <cz/format.hpp>

using namespace cz;

TEST_CASE("format works") {
    Aligned_Buffer<32> buffer;
    Arena arena;
    arena.init(buffer.mem());
    auto string = format(arena.allocator(), 123, " + ", 456);

    REQUIRE(string == "123 + 456");
}
