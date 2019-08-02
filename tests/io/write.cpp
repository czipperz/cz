#include "../catch.hpp"

#include "../../src/defer.hpp"
#include "../../src/io/write.hpp"

using namespace cz;
using namespace cz::io;

TEST_CASE("concat works") {
    auto string = concat(123, " + ", 456);
    CZ_DEFER(string.drop());

    REQUIRE(string == "123 + 456");
}

TEST_CASE("write multiple arguments works") {
    String string;
    CZ_DEFER(string.drop());

    Writer writer = string_writer(&string);
    write(writer, 123, " + ", 456);

    REQUIRE(string == "123 + 456");
}

TEST_CASE("write multiple arguments with debug") {
    String string;
    CZ_DEFER(string.drop());

    Writer writer = string_writer(&string);
    int elems[3] = {10, 30, 40};
    write(writer, "  ", debug(slice(elems)));

    REQUIRE(string == "  [10, 30, 40]");
}

TEST_CASE("write str") {
    String string;
    CZ_DEFER(string.drop());

    Writer writer = string_writer(&string);
    Result result;
    SECTION("use Writer::write_str") { result = writer.write_str("abc"); }
    SECTION("use Writer::write_str") { result = write(writer, "abc"); }

    REQUIRE(string == "abc");
    REQUIRE(result.is_ok());
}

TEST_CASE("write char") {
    String string;
    CZ_DEFER(string.drop());

    Writer writer = string_writer(&string);
    Result result = write(writer, 'a');

    REQUIRE(string == "a");
    REQUIRE(result.is_ok());
}

TEST_CASE("write(int = 123)") {
    String string;
    CZ_DEFER(string.drop());

    Writer writer = string_writer(&string);
    Result result = write(writer, 123);

    REQUIRE(string == "123");
    REQUIRE(result.is_ok());
}

TEST_CASE("write(int = 0)") {
    String string;
    CZ_DEFER(string.drop());

    Writer writer = string_writer(&string);
    Result result = write(writer, 0);

    REQUIRE(string == "0");
    REQUIRE(result.is_ok());
}

TEST_CASE("write(int = 9)") {
    String string;
    CZ_DEFER(string.drop());

    Writer writer = string_writer(&string);
    Result result = write(writer, 9);

    REQUIRE(string == "9");
    REQUIRE(result.is_ok());
}

TEST_CASE("write(Address(NULL))") {
    String string;
    CZ_DEFER(string.drop());

    Writer writer = string_writer(&string);
    Result result = write(writer, addr(NULL));

    REQUIRE(string == "NULL");
    REQUIRE(result.is_ok());
}

TEST_CASE("write(Address(arbitrary) starts with 0x)") {
    String string;
    CZ_DEFER(string.drop());

    Writer writer = string_writer(&string);
    Result result = write(writer, addr(&string));

    REQUIRE(string[0] == '0');
    REQUIRE(string[1] == 'x');
    REQUIRE(string.len() > 2);
    REQUIRE(result.is_ok());
}