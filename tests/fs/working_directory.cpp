#include "../test_base.hpp"

#include <cz/defer.hpp>
#include <cz/fs/working_directory.hpp>
#include <cz/mem/heap.hpp>

using namespace cz;
using namespace cz::fs;

TEST_CASE("get_working_directory() returns non-empty") {
    String path;
    mem::Allocator allocator = mem::heap_allocator();
    CZ_DEFER(path.drop(allocator));

    REQUIRE(!cz::is_err(get_working_directory(allocator, &path)));

    REQUIRE(path != "");
}
