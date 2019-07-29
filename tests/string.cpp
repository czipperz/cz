#include "catch.hpp"

#include "../src/string.hpp"

using cz::Str;
using cz::String;

TEST_CASE("Str::Str(const char*)"){
    Str str("abc");

    REQUIRE(str == "abc");
    REQUIRE(str.len == 3);
}

TEST_CASE("Str::cstr") {
    auto str = Str::cstr("abc");

    REQUIRE(str == "abc");
    REQUIRE(str.len == 3);
}

TEST_CASE("String==Str same length") {
    char buffer[5] = "abcd";
    String string(buffer, 4);
    Str str(buffer, 4);

    REQUIRE(string == str);
    REQUIRE(str == string);
}

TEST_CASE("String==Str shorter string") {
    char buffer[5] = "abcd";
    String string(buffer, 3);
    Str str(buffer, 4);

    REQUIRE(string != str);
    REQUIRE(str != string);
}

TEST_CASE("String==Str longer string") {
    char buffer[5] = "abcd";
    String string(buffer, 4);
    Str str(buffer, 3);

    REQUIRE(string != str);
    REQUIRE(str != string);
}
