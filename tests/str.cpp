#include "catch.hpp"

#include "../src/str.hpp"

using cz::Str;

TEST_CASE("Str::Str() is empty") {
    Str str;

    REQUIRE(str == "");
}

TEST_CASE("Str::Str(const char*)") {
    Str str("abc");

    REQUIRE(str == "abc");
    REQUIRE(str.len == 3);
}

TEST_CASE("Str::cstr") {
    auto str = Str::cstr("abc");

    REQUIRE(str == "abc");
    REQUIRE(str.len == 3);
}
