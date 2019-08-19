#include "test_base.hpp"

#include <cz/str.hpp>

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

TEST_CASE("Str::find(char) matches") {
    Str str("abc");
    REQUIRE(str.find('a') == str.buffer);
    REQUIRE(str.find('b') == str.buffer + 1);
    REQUIRE(str.find('c') == str.buffer + 2);
}

TEST_CASE("Str::find(char) goes in forward order") {
    Str str("abcabcabc");
    REQUIRE(str.find('a') == str.buffer);
}

TEST_CASE("Str::find(char) is case insensitive") {
    Str str("aBcabcabc");
    REQUIRE(str.find('b') == str.buffer + 4);
}

TEST_CASE("Str::find(char) no match") {
    Str str("abc");
    REQUIRE(str.find('d') == nullptr);
}

TEST_CASE("Str::find('\0') doesn't run over end") {
    Str str("abc");
    REQUIRE(str.find('\0') == nullptr);
}

TEST_CASE("Str::rfind(char) matches") {
    Str str("abc");
    REQUIRE(str.rfind('a') == str.buffer);
    REQUIRE(str.rfind('b') == str.buffer + 1);
    REQUIRE(str.rfind('c') == str.buffer + 2);
}

TEST_CASE("Str::rfind(char) goes in reverse order") {
    Str str("abcabcabc");
    REQUIRE(str.rfind('a') == str.buffer + 6);
}

TEST_CASE("Str::rfind(char) is case insensitive") {
    Str str("abcabcaBc");
    REQUIRE(str.rfind('b') == str.buffer + 4);
}

TEST_CASE("Str::rfind(char) no match") {
    Str str("abc");
    REQUIRE(str.rfind('d') == nullptr);
}

TEST_CASE("Str::rfind('\0') doesn't run over end") {
    Str str("abc");
    REQUIRE(str.rfind('\0') == nullptr);
}
