#include "../test_base.hpp"

#include <cz/defer.hpp>
#include <cz/fs/directory.hpp>
#include <cz/mem/heap.hpp>

using namespace cz;
using namespace cz::mem;
using namespace cz::fs;

void set_wd();

TEST_CASE("fs::files works") {
    set_wd();

    Allocator allocator = heap_allocator();

    const char* dir = "tests/fs";

    DirectoryIterator iterator(allocator);
    REQUIRE(iterator.create(dir).is_ok());
    CZ_DEFER(REQUIRE(iterator.destroy().is_ok()));

    SmallVector<String, 0> paths;
    CZ_DEFER(paths.drop(allocator));
    CZ_DEFER(for (size_t i = 0; i < paths.len(); ++i) { paths[i].drop(allocator); });
    REQUIRE(files(allocator, dir, &paths).is_ok());

    size_t i = 0;
    while (!iterator.done()) {
        cz::io::put("ls: ", iterator.file());
        REQUIRE(iterator.file() == paths[i++]);
        REQUIRE(iterator.advance().is_ok());
    }
}

TEST_CASE("fs::directory_component() empty input empty output") {
    REQUIRE(directory_component("") == "");
}

TEST_CASE("fs::directory_component() no directory is empty output") {
    REQUIRE(directory_component("abc") == "");
}

TEST_CASE("fs::directory_component() gets directory subset") {
    REQUIRE(directory_component("abc/def.txt") == "abc/");
}

TEST_CASE("fs::directory_component() gets directory subset multiple directories") {
    REQUIRE(directory_component("abc/def/ghi") == "abc/def/");
}

TEST_CASE("fs::directory_component() trailing slash returns input") {
    REQUIRE(directory_component("abc/def/") == "abc/def/");
}
