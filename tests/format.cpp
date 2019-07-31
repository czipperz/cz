#include "catch.hpp"

#include "../src/defer.hpp"
#include "../src/format.hpp"

using cz::String;
using namespace cz::format;

TEST_CASE("write str") {
    String string;
    CZ_DEFER(string.drop());

    Writer writer = string_writer(&string);
    Result result;
    SECTION("use Writer::write_str") { result = writer.write_str("abc"); }
    SECTION("use Writer::write_str") { result = write(writer, "abc"); }

    REQUIRE(string == "abc");
    REQUIRE(result == Result::Ok);
}

TEST_CASE("write char") {
    String string;
    CZ_DEFER(string.drop());

    Writer writer = string_writer(&string);
    Result result;
    SECTION("use Writer::write_char") { result = writer.write_char('a'); }
    SECTION("use write(char)") { result = write(writer, 'a'); }

    REQUIRE(string == "a");
    REQUIRE(result == Result::Ok);
}

TEST_CASE("write(int = 123)") {
    String string;
    CZ_DEFER(string.drop());

    Writer writer = string_writer(&string);
    Result result = write(writer, 123);

    REQUIRE(string == "123");
    REQUIRE(result == Result::Ok);
}

TEST_CASE("write(int = 0)") {
    String string;
    CZ_DEFER(string.drop());

    Writer writer = string_writer(&string);
    Result result = write(writer, 0);

    REQUIRE(string == "0");
    REQUIRE(result == Result::Ok);
}

TEST_CASE("write(int = 9)") {
    String string;
    CZ_DEFER(string.drop());

    Writer writer = string_writer(&string);
    Result result = write(writer, 9);

    REQUIRE(string == "9");
    REQUIRE(result == Result::Ok);
}
