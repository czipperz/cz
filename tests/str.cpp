#include "test_base.hpp"

#include "cz/str.hpp"

using cz::Str;

TEST_CASE("Str::Str() is empty") {
    Str str;

    CHECK(str == "");
    CHECK(str.len == 0);
}

TEST_CASE("Str::Str(\"\")") {
    Str str("");

    CHECK(str == "");
    CHECK(str.len == 0);
}

TEST_CASE("Str::Str(const char*)") {
    Str str("abc");

    CHECK(str == "abc");
    CHECK(str.len == 3);
}

TEST_CASE("Str::cstr()") {
    auto str = Str::cstr("abc");

    CHECK(str == "abc");
    CHECK(str.len == 3);
}
