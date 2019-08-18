#include "../test_base.hpp"

#include <cz/defer.hpp>
#include <cz/io/write.hpp>
#include <cz/mem.hpp>
#include "../context.hpp"

using namespace cz;
using namespace cz::io;
using cz::test::ctxt;

TEST_CASE("write multiple arguments works") {
    mem::AlignedBuffer<32> buffer;
    mem::Arena arena;
    arena.mem = buffer;
    mem::Allocated<String> string = {{}, arena.allocator()};

    Writer writer = string_writer(&string);
    write(writer, 123, " + ", 456);

    REQUIRE(string.object == "123 + 456");
}

TEST_CASE("write multiple arguments with debug") {
    mem::AlignedBuffer<32> buffer;
    mem::Arena arena;
    arena.mem = buffer;
    mem::Allocated<String> string = {{}, arena.allocator()};

    Writer writer = string_writer(&string);
    int elems[3] = {10, 30, 40};
    write(writer, "  ", format::debug(slice(elems)));

    REQUIRE(string.object == "  [10, 30, 40]");
}

TEST_CASE("write str") {
    mem::AlignedBuffer<32> buffer;
    mem::Arena arena;
    arena.mem = buffer;
    mem::Allocated<String> string = {{}, arena.allocator()};

    Writer writer = string_writer(&string);
    Result result;
    SECTION("use Writer::write_str") { result = writer.write_str("abc"); }
    SECTION("use Writer::write_str") { result = write(writer, "abc"); }

    REQUIRE(string.object == "abc");
    REQUIRE(result.is_ok());
}

TEST_CASE("write char") {
    mem::AlignedBuffer<32> buffer;
    mem::Arena arena;
    arena.mem = buffer;
    mem::Allocated<String> string = {{}, arena.allocator()};

    Writer writer = string_writer(&string);
    Result result = write(writer, 'a');

    REQUIRE(string.object == "a");
    REQUIRE(result.is_ok());
}

TEST_CASE("write(int = 123)") {
    mem::AlignedBuffer<32> buffer;
    mem::Arena arena;
    arena.mem = buffer;
    mem::Allocated<String> string = {{}, arena.allocator()};

    Writer writer = string_writer(&string);
    Result result = write(writer, 123);

    REQUIRE(string.object == "123");
    REQUIRE(result.is_ok());
}

TEST_CASE("write(int = 0)") {
    mem::AlignedBuffer<32> buffer;
    mem::Arena arena;
    arena.mem = buffer;
    mem::Allocated<String> string = {{}, arena.allocator()};

    Writer writer = string_writer(&string);
    Result result = write(writer, 0);

    REQUIRE(string.object == "0");
    REQUIRE(result.is_ok());
}

TEST_CASE("write(int = 9)") {
    mem::AlignedBuffer<32> buffer;
    mem::Arena arena;
    arena.mem = buffer;
    mem::Allocated<String> string = {{}, arena.allocator()};

    Writer writer = string_writer(&string);
    Result result = write(writer, 9);

    REQUIRE(string.object == "9");
    REQUIRE(result.is_ok());
}

TEST_CASE("write(int = -47)") {
    mem::AlignedBuffer<32> buffer;
    mem::Arena arena;
    arena.mem = buffer;
    mem::Allocated<String> string = {{}, arena.allocator()};

    Writer writer = string_writer(&string);
    Result result = write(writer, -47);

    REQUIRE(string.object == "-47");
    REQUIRE(result.is_ok());
}

TEST_CASE("write(Address(nullptr))") {
    mem::AlignedBuffer<32> buffer;
    mem::Arena arena;
    arena.mem = buffer;
    mem::Allocated<String> string = {{}, arena.allocator()};

    Writer writer = string_writer(&string);
    Result result = write(writer, format::addr(nullptr));

    REQUIRE(string.object == "nullptr");
    REQUIRE(result.is_ok());
}

TEST_CASE("write(Address(arbitrary) starts with 0x)") {
    mem::AlignedBuffer<32> buffer;
    mem::Arena arena;
    arena.mem = buffer;
    mem::Allocated<String> string = {{}, arena.allocator()};

    Writer writer = string_writer(&string);
    Result result = write(writer, format::addr(&string));

    REQUIRE(string.object[0] == '0');
    REQUIRE(string.object[1] == 'x');
    REQUIRE(string.object.len() > 2);
    REQUIRE(result.is_ok());
}
