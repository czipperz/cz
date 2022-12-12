#include <czt/test_base.hpp>

#include <cz/path.hpp>

using namespace cz;
using namespace cz::path;

TEST_CASE("path::directory_component() empty input no output") {
    size_t end;
    CHECK_FALSE(directory_component("", &end));
}

TEST_CASE("path::directory_component() no directory no output") {
    size_t end;
    CHECK_FALSE(directory_component("abc", &end));
}

TEST_CASE("path::directory_component() gets directory subset") {
    cz::Str dc;
    REQUIRE(directory_component("abc/def.txt", &dc));
    CHECK(dc == "abc");
}

TEST_CASE("path::directory_component() gets directory subset multiple directories") {
    cz::Str dc;
    REQUIRE(directory_component("abc/def/ghi", &dc));
    CHECK(dc == "abc/def");
}

TEST_CASE("path::directory_component() trailing slash returns input") {
    cz::Str dc;
    REQUIRE(directory_component("abc/def/", &dc));
    CHECK(dc == "abc/def");
}

TEST_CASE("path::directory_component() absolute path") {
    cz::Str dc;
    REQUIRE(directory_component("/abc/def/", &dc));
    CHECK(dc == "/abc/def");

    REQUIRE(directory_component("/abc/def", &dc));
    CHECK(dc == "/abc");

    REQUIRE(directory_component("/abc", &dc));
    CHECK(dc == "/");

    REQUIRE(directory_component("/", &dc));
    CHECK(dc == "/");

#ifdef _WIN32
    REQUIRE(directory_component("c:/abc", &dc));
    CHECK(dc == "c:/");
#endif
}

TEST_CASE("path::pop_name() absolute path") {
    cz::Str input;
    size_t end;

    input = "/abc/def/";
    REQUIRE(pop_name(input, &end));
    CHECK(input.slice_end(end) == "/abc/def/");

    input = "/abc/def";
    REQUIRE(pop_name(input, &end));
    CHECK(input.slice_end(end) == "/abc/");

    input = "/abc";
    REQUIRE(pop_name(input, &end));
    CHECK(input.slice_end(end) == "/");

    input = "/";
    REQUIRE(pop_name(input, &end));
    CHECK(input.slice_end(end) == "/");

#ifdef _WIN32
    input = "c:/abc";
    REQUIRE(pop_name(input, &end));
    CHECK(input.slice_end(end) == "c:/");
#endif
}

TEST_CASE("path::name_component() empty input no output") {
    cz::Str dc;
    REQUIRE(name_component("", &dc));
    CHECK(dc == "");
}

TEST_CASE("path::name_component() no directory is name output") {
    cz::Str dc;
    REQUIRE(name_component("abc", &dc));
    CHECK(dc == "abc");
}

TEST_CASE("path::name_component() gets name subset") {
    cz::Str dc;
    REQUIRE(name_component("abc/def.txt", &dc));
    CHECK(dc == "def.txt");
}

TEST_CASE("path::name_component() gets name subset multiple directories") {
    cz::Str dc;
    REQUIRE(name_component("abc/def/ghi", &dc));
    CHECK(dc == "ghi");
}

TEST_CASE("path::name_component() trailing slash no output") {
    cz::Str dc;
    CHECK_FALSE(name_component("abc/def/", &dc));
}

TEST_CASE("path::flatten() empty input empty output") {
    char buffer[3] = {};
    size_t size = 0;

    flatten(buffer, &size);

    CHECK(buffer[0] == 0);
    CHECK(buffer[1] == 0);
    CHECK(buffer[2] == 0);
    CHECK(size == 0);
}

TEST_CASE("path::flatten() no directory is empty output") {
    char buffer[] = "ooga_booga.txt";
    size_t size = sizeof(buffer) - 1;

    flatten(buffer, &size);

    REQUIRE(Str{buffer, size} == "ooga_booga.txt");
}

TEST_CASE("path::flatten() single directory does nothing") {
    char buffer[] = "folder/ooga_booga.txt";
    size_t size = sizeof(buffer) - 1;

    flatten(buffer, &size);

    REQUIRE(Str{buffer, size} == "folder/ooga_booga.txt");
}

TEST_CASE("path::flatten() .. works in middle") {
    char buffer[] = "folder/../ooga_booga.txt";
    size_t size = sizeof(buffer) - 1;

    flatten(buffer, &size);

    REQUIRE(Str{buffer, size} == "ooga_booga.txt");
}

TEST_CASE("path::flatten() .. chain works in middle") {
    char buffer[] = "folder1/folder2/../../ooga_booga.txt";
    size_t size = sizeof(buffer) - 1;

    flatten(buffer, &size);

    REQUIRE(Str{buffer, size} == "ooga_booga.txt");
}

TEST_CASE("path::flatten() .. chain partially removed in middle") {
    char buffer[] = "folder/../../ooga_booga.txt";
    size_t size = sizeof(buffer) - 1;

    flatten(buffer, &size);

    REQUIRE(Str{buffer, size} == "../ooga_booga.txt");
}

TEST_CASE("path::flatten() .. at end resolves to empty string") {
    char buffer[] = "ooga_booga/..";
    size_t size = sizeof(buffer) - 1;

    flatten(buffer, &size);

    REQUIRE(Str{buffer, size} == "");
}

TEST_CASE("path::flatten() .. works at end no trailing slash") {
    char buffer[] = "folder/ooga_booga/..";
    size_t size = sizeof(buffer) - 1;

    flatten(buffer, &size);

    REQUIRE(Str{buffer, size} == "folder/");
}

TEST_CASE("path::flatten() .. works at end with trailing /") {
    char buffer[] = "folder/ooga_booga/../";
    size_t size = sizeof(buffer) - 1;

    flatten(buffer, &size);

    REQUIRE(Str{buffer, size} == "folder/");
}

TEST_CASE("path::flatten() . works middle") {
    char buffer[] = "folder/./ooga_booga";
    size_t size = sizeof(buffer) - 1;

    flatten(buffer, &size);

    REQUIRE(Str{buffer, size} == "folder/ooga_booga");
}

TEST_CASE("path::flatten() . works end") {
    char buffer[] = "folder/ooga_booga/.";
    size_t size = sizeof(buffer) - 1;

    flatten(buffer, &size);

    REQUIRE(Str{buffer, size} == "folder/ooga_booga/");
}

TEST_CASE("path::flatten() . works end with trailing /") {
    char buffer[] = "folder/ooga_booga/./";
    size_t size = sizeof(buffer) - 1;

    flatten(buffer, &size);

    REQUIRE(Str{buffer, size} == "folder/ooga_booga/");
}

TEST_CASE("path::flatten() . then .. works in middle") {
    char buffer[] = "folder/./../ooga_booga.txt";
    size_t size = sizeof(buffer) - 1;

    flatten(buffer, &size);

    REQUIRE(Str{buffer, size} == "ooga_booga.txt");
}

TEST_CASE("path::flatten() .. then . works in middle") {
    char buffer[] = "folder/.././ooga_booga.txt";
    size_t size = sizeof(buffer) - 1;

    flatten(buffer, &size);

    REQUIRE(Str{buffer, size} == "ooga_booga.txt");
}

TEST_CASE("path::flatten() ... unchanged") {
    char buffer[] = "folder/.../ooga_booga.txt";
    size_t size = sizeof(buffer) - 1;

    flatten(buffer, &size);

    REQUIRE(Str{buffer, size} == "folder/.../ooga_booga.txt");
}

TEST_CASE("path::flatten() starting with ./ stripped") {
    char buffer[] = "./ooga_booga.txt";
    size_t size = sizeof(buffer) - 1;

    flatten(buffer, &size);

    REQUIRE(Str{buffer, size} == "ooga_booga.txt");
}

TEST_CASE("path::flatten() starting with ../ unchanged") {
    char buffer[] = "../ooga_booga.txt";
    size_t size = sizeof(buffer) - 1;

    flatten(buffer, &size);

    REQUIRE(Str{buffer, size} == "../ooga_booga.txt");
}

TEST_CASE("path::flatten() starting with multiple ../ unchanged") {
    char buffer[] = "../../ooga_booga.txt";
    size_t size = sizeof(buffer) - 1;

    flatten(buffer, &size);

    REQUIRE(Str{buffer, size} == "../../ooga_booga.txt");
}

#ifdef _WIN32
TEST_CASE("path::flatten() absolute path with drive windows") {
    char buffer[] = "c:/../abc";
    size_t size = sizeof(buffer) - 1;

    flatten(buffer, &size);

    REQUIRE(Str{buffer, size} == "c:/abc");
}

TEST_CASE("path::flatten() relative path with drive windows") {
    char buffer[] = "c:../abc";
    size_t size = sizeof(buffer) - 1;

    flatten(buffer, &size);

    REQUIRE(Str{buffer, size} == "c:../abc");
}
#endif

TEST_CASE("path::flatten() absolute path") {
    char buffer[] = "/../abc";
    size_t size = sizeof(buffer) - 1;

    flatten(buffer, &size);

    REQUIRE(Str{buffer, size} == "/abc");
}

TEST_CASE("path::flatten() absolute path 2") {
    char buffer[] = "/..";
    size_t size = sizeof(buffer) - 1;

    flatten(buffer, &size);

    REQUIRE(Str{buffer, size} == "/");
}

#ifdef _WIN32
TEST_CASE("path::flatten() relative path with drive .. test") {
    char buffer[] = "x:abc/../def";
    size_t size = sizeof(buffer) - 1;

    flatten(buffer, &size);

    REQUIRE(Str{buffer, size} == "x:def");
}

TEST_CASE("path::flatten() unc path basic test") {
    char buffer[] = "//abc/def/123/../ok";
    size_t size = sizeof(buffer) - 1;

    flatten(buffer, &size);

    REQUIRE(Str{buffer, size} == "//abc/def/ok");
}

TEST_CASE("path::flatten() unc path .. at start 1") {
    char buffer[] = "//../abc/ok";
    size_t size = sizeof(buffer) - 1;

    flatten(buffer, &size);

    REQUIRE(Str{buffer, size} == "//../abc/ok");
}

TEST_CASE("path::flatten() unc path .. at start 2") {
    char buffer[] = "//abc/../ok";
    size_t size = sizeof(buffer) - 1;

    flatten(buffer, &size);

    REQUIRE(Str{buffer, size} == "//abc/../ok");
}

TEST_CASE("path::flatten() unc path .. at start 3") {
    char buffer[] = "//abc/def/../ok";
    size_t size = sizeof(buffer) - 1;

    flatten(buffer, &size);

    REQUIRE(Str{buffer, size} == "//abc/def/ok");
}
#endif

TEST_CASE("path::flatten() /// at start are collapsed") {
    char buffer[] = "///abc";
    size_t size = sizeof(buffer) - 1;

    flatten(buffer, &size);

#ifdef _WIN32
    // UNC paths are only handled on Windows.
    REQUIRE(Str{buffer, size} == "//abc");
#else
    REQUIRE(Str{buffer, size} == "/abc");
#endif
}

TEST_CASE("path::flatten() /// in middle are collapsed") {
    char buffer[] = "def///abc";
    size_t size = sizeof(buffer) - 1;

    flatten(buffer, &size);

    REQUIRE(Str{buffer, size} == "def/abc");
}

TEST_CASE("path::flatten() /// at end are collapsed") {
    char buffer[] = "def///";
    size_t size = sizeof(buffer) - 1;

    flatten(buffer, &size);

    REQUIRE(Str{buffer, size} == "def/");
}
