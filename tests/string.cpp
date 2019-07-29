#include "catch.hpp"

#include "../src/string.hpp"

using cz::Str;

TEST_CASE("Str::cstr run time"){
    const char* cstr = "abc";

    auto str = Str::from(cstr);

    REQUIRE(str.len == 3);
}

TEST_CASE("Str::cstr compile time") {
    auto str = Str::cstr("abc");

    REQUIRE(str.len == 3);
}
