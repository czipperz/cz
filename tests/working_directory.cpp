#include <czt/test_base.hpp>

#include <cz/defer.hpp>
#include <cz/heap.hpp>
#include <cz/working_directory.hpp>

using namespace cz;

TEST_CASE("get_working_directory() returns non-empty") {
    String path;
    Allocator allocator = heap_allocator();
    CZ_DEFER(path.drop(allocator));

    REQUIRE(!cz::is_err(get_working_directory(allocator, &path)));

    REQUIRE(path != "");
}
