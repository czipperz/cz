#include "../catch.hpp"

#include "../../src/defer.hpp"
#include "../../src/io/write.hpp"
#include "../../src/mem.hpp"
#include "../context.hpp"

using namespace cz;
using namespace cz::io;
using cz::test::ctxt;

TEST_CASE("concat works") {
    mem::StackArena<32> arena;
    C c = ctxt(arena.allocator());

    auto string = concat(&c, 123, " + ", 456);
    CZ_DEFER(string.drop(&c));

    REQUIRE(string == "123 + 456");
}

TEST_CASE("write multiple arguments works") {
    mem::StackArena<32> arena;
    C c = ctxt(arena.allocator());

    String string;
    CZ_DEFER(string.drop(&c));

    Writer writer = string_writer(&string);
    write(&c, writer, 123, " + ", 456);

    REQUIRE(string == "123 + 456");
}

TEST_CASE("write multiple arguments with debug") {
    mem::StackArena<32> arena;
    C c = ctxt(arena.allocator());

    String string;
    CZ_DEFER(string.drop(&c));

    Writer writer = string_writer(&string);
    int elems[3] = {10, 30, 40};
    write(&c, writer, "  ", debug(slice(elems)));

    REQUIRE(string == "  [10, 30, 40]");
}

TEST_CASE("write str") {
    mem::StackArena<32> arena;
    C c = ctxt(arena.allocator());

    String string;
    CZ_DEFER(string.drop(&c));

    Writer writer = string_writer(&string);
    Result result;
    SECTION("use Writer::write_str") { result = writer.write_str(&c, "abc"); }
    SECTION("use Writer::write_str") { result = write(&c, writer, "abc"); }

    REQUIRE(string == "abc");
    REQUIRE(result.is_ok());
}

TEST_CASE("write char") {
    mem::StackArena<32> arena;
    C c = ctxt(arena.allocator());

    String string;
    CZ_DEFER(string.drop(&c));

    Writer writer = string_writer(&string);
    Result result = write(&c, writer, 'a');

    REQUIRE(string == "a");
    REQUIRE(result.is_ok());
}

TEST_CASE("write(int = 123)") {
    mem::StackArena<32> arena;
    C c = ctxt(arena.allocator());

    String string;
    CZ_DEFER(string.drop(&c));

    Writer writer = string_writer(&string);
    Result result = write(&c, writer, 123);

    REQUIRE(string == "123");
    REQUIRE(result.is_ok());
}

TEST_CASE("write(int = 0)") {
    mem::StackArena<32> arena;
    C c = ctxt(arena.allocator());

    String string;
    CZ_DEFER(string.drop(&c));

    Writer writer = string_writer(&string);
    Result result = write(&c, writer, 0);

    REQUIRE(string == "0");
    REQUIRE(result.is_ok());
}

TEST_CASE("write(int = 9)") {
    mem::StackArena<32> arena;
    C c = ctxt(arena.allocator());

    String string;
    CZ_DEFER(string.drop(&c));

    Writer writer = string_writer(&string);
    Result result = write(&c, writer, 9);

    REQUIRE(string == "9");
    REQUIRE(result.is_ok());
}

TEST_CASE("write(int = -47)") {
    mem::StackArena<32> arena;
    C c = ctxt(arena.allocator());

    String string;
    CZ_DEFER(string.drop(&c));

    Writer writer = string_writer(&string);
    Result result = write(&c, writer, -47);

    REQUIRE(string == "-47");
    REQUIRE(result.is_ok());
}

TEST_CASE("write(Address(NULL))") {
    mem::StackArena<32> arena;
    C c = ctxt(arena.allocator());

    String string;
    CZ_DEFER(string.drop(&c));

    Writer writer = string_writer(&string);
    Result result = write(&c, writer, addr(NULL));

    REQUIRE(string == "NULL");
    REQUIRE(result.is_ok());
}

TEST_CASE("write(Address(arbitrary) starts with 0x)") {
    mem::StackArena<32> arena;
    C c = ctxt(arena.allocator());

    String string;
    CZ_DEFER(string.drop(&c));

    Writer writer = string_writer(&string);
    Result result = write(&c, writer, addr(&string));

    REQUIRE(string[0] == '0');
    REQUIRE(string[1] == 'x');
    REQUIRE(string.len() > 2);
    REQUIRE(result.is_ok());
}
