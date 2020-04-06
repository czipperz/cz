#include <czt/test_base.hpp>

#include <cz/aligned_buffer.hpp>
#include <cz/arena.hpp>
#include <cz/defer.hpp>
#include <cz/write.hpp>

using namespace cz;

TEST_CASE("write multiple arguments works") {
    Aligned_Buffer<32> buffer;
    Arena arena;
    arena.mem = buffer;
    AllocatedString string = {};
    string.allocator = arena.allocator();

    Writer writer = string_writer(&string);
    write(writer, 123, " + ", 456);

    REQUIRE(string == "123 + 456");
}

TEST_CASE("write multiple arguments with debug") {
    Aligned_Buffer<32> buffer;
    Arena arena;
    arena.mem = buffer;
    AllocatedString string = {};
    string.allocator = arena.allocator();

    Writer writer = string_writer(&string);
    int elems[3] = {10, 30, 40};
    write(writer, "  ", format::debug(slice(elems)));

    REQUIRE(string == "  [10, 30, 40]");
}

TEST_CASE("write str") {
    Aligned_Buffer<32> buffer;
    Arena arena;
    arena.mem = buffer;
    AllocatedString string = {};
    string.allocator = arena.allocator();

    Writer writer = string_writer(&string);
    Result result;
    SECTION("use Writer::write_str") { result = write(writer, "abc"); }

    REQUIRE(string == "abc");
    REQUIRE(result.is_ok());
}

TEST_CASE("write char") {
    Aligned_Buffer<32> buffer;
    Arena arena;
    arena.mem = buffer;
    AllocatedString string = {};
    string.allocator = arena.allocator();

    Writer writer = string_writer(&string);
    Result result = write(writer, 'a');

    REQUIRE(string == "a");
    REQUIRE(result.is_ok());
}

TEST_CASE("write(int = 123)") {
    Aligned_Buffer<32> buffer;
    Arena arena;
    arena.mem = buffer;
    AllocatedString string = {};
    string.allocator = arena.allocator();

    Writer writer = string_writer(&string);
    Result result = write(writer, 123);

    REQUIRE(string == "123");
    REQUIRE(result.is_ok());
}

TEST_CASE("write(int = 0)") {
    Aligned_Buffer<32> buffer;
    Arena arena;
    arena.mem = buffer;
    AllocatedString string = {};
    string.allocator = arena.allocator();

    Writer writer = string_writer(&string);
    Result result = write(writer, 0);

    REQUIRE(string == "0");
    REQUIRE(result.is_ok());
}

TEST_CASE("write(int = 9)") {
    Aligned_Buffer<32> buffer;
    Arena arena;
    arena.mem = buffer;
    AllocatedString string = {};
    string.allocator = arena.allocator();

    Writer writer = string_writer(&string);
    Result result = write(writer, 9);

    REQUIRE(string == "9");
    REQUIRE(result.is_ok());
}

TEST_CASE("write(int = -47)") {
    Aligned_Buffer<32> buffer;
    Arena arena;
    arena.mem = buffer;
    AllocatedString string = {};
    string.allocator = arena.allocator();

    Writer writer = string_writer(&string);
    Result result = write(writer, -47);

    REQUIRE(string == "-47");
    REQUIRE(result.is_ok());
}

TEST_CASE("write(Address(nullptr))") {
    Aligned_Buffer<32> buffer;
    Arena arena;
    arena.mem = buffer;
    AllocatedString string = {};
    string.allocator = arena.allocator();

    Writer writer = string_writer(&string);
    Result result = write(writer, format::addr(nullptr));

    REQUIRE(string == "nullptr");
    REQUIRE(result.is_ok());
}

TEST_CASE("write(Address(arbitrary) starts with 0x)") {
    Aligned_Buffer<32> buffer;
    Arena arena;
    arena.mem = buffer;
    AllocatedString string = {};
    string.allocator = arena.allocator();

    Writer writer = string_writer(&string);
    Result result = write(writer, format::addr(&string));

    REQUIRE(string[0] == '0');
    REQUIRE(string[1] == 'x');
    REQUIRE(string.len() > 2);
    REQUIRE(result.is_ok());
}
