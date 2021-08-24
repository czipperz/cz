#include <czt/test_base.hpp>

#include <cz/defer.hpp>
#include <cz/directory.hpp>
#include <cz/heap.hpp>
#include <cz/string.hpp>
#include <cz/working_directory.hpp>

using namespace cz;

void set_wd() {
    static bool already_set = false;
    if (already_set) {
        return;
    }

    Allocator allocator = heap_allocator();
    String path = {};
    CZ_DEFER(path.drop(allocator));

    REQUIRE(!cz::is_err(get_working_directory(allocator, &path)));

    printf("cwd: %s\n", path.buffer);

#if _WIN32
    Str end = "/out/build/x64-Debug";
    if (path.ends_with(end)) {
        path.len = path.len - end.len;
        path.push('\0');
        set_working_directory(path.buffer);
    }
#endif

    already_set = true;
}

TEST_CASE("files works") {
    set_wd();

    Allocator allocator = heap_allocator();

    const char* dir = "tests";
    cz::String file = {};
    CZ_DEFER(file.drop(allocator));

    Directory_Iterator iterator;
    REQUIRE(iterator.init(dir, allocator, &file).is_ok());
    CZ_DEFER(REQUIRE(iterator.drop().is_ok()));

    Vector<String> paths = {};
    CZ_DEFER(paths.drop(allocator));
    CZ_DEFER(for (size_t i = 0; i < paths.len; ++i) { paths[i].drop(allocator); });
    REQUIRE(files(allocator, allocator, dir, &paths).is_ok());

    size_t i = 0;
    while (!iterator.done()) {
        fputs("ls: ", stdout);
        fwrite(file.buffer, 1, file.len, stdout);
        putchar('\n');
        REQUIRE(file == paths[i++]);
        file.len = 0;
        REQUIRE(iterator.advance(allocator, &file).is_ok());
    }
}
