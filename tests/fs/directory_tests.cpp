#include <czt/test_base.hpp>

#include <cz/defer.hpp>
#include <cz/fs/directory.hpp>
#include <cz/heap.hpp>
#include <cz/string.hpp>

using namespace cz;
using namespace cz::fs;

void set_wd();

TEST_CASE("fs::files works") {
    set_wd();

    Allocator allocator = heap_allocator();

    const char* dir = "tests/fs";

    Directory_Iterator iterator;
    REQUIRE(iterator.init(allocator, dir).is_ok());
    CZ_DEFER(REQUIRE(iterator.drop(allocator).is_ok()));

    Vector<String> paths = {};
    CZ_DEFER(paths.drop(allocator));
    CZ_DEFER(for (size_t i = 0; i < paths.len(); ++i) { paths[i].drop(allocator); });
    REQUIRE(files(allocator, allocator, dir, &paths).is_ok());

    size_t i = 0;
    while (!iterator.done()) {
        fputs("ls: ", stdout);
        cz::Str file = iterator.file();
        fwrite(file.buffer, 1, file.len, stdout);
        putchar('\n');
        REQUIRE(iterator.file() == paths[i++]);
        REQUIRE(iterator.advance(allocator).is_ok());
    }
}
