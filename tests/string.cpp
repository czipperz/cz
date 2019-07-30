#include "catch.hpp"

#include "../src/defer.hpp"
#include "../src/mem.hpp"
#include "../src/string.hpp"
#include "mock_allocator.hpp"

using cz::Str;
using cz::String;
using namespace cz::mem;

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

TEST_CASE("String::String() is empty") {
    String string;

    REQUIRE(string == "");
}

TEST_CASE("String::String(char*, size_t)") {
    char buffer[4] = "abc";
    String string(buffer, 2);

    REQUIRE(string == "ab");
    REQUIRE(string.cap() == 2);
}

TEST_CASE("String::String(char*, size_t, size_t)") {
    char buffer[4] = "abc";
    String string(buffer, 2, 4);

    REQUIRE(string == "ab");
    REQUIRE(string.cap() == 4);
}

TEST_CASE("String::String(Str) clones") {
    char buffer[3];

    test::MockAllocator test = {buffer, NULL, 0, 3};
    with_global_allocator(test, [&]() { String string("abc"); });
}

TEST_CASE("String::append from empty string") {
    String string;
    CZ_DEFER(string.drop());
    string.append("abc");
    REQUIRE(string == "abc");
}

TEST_CASE("String::append from non-empty string and reallocates") {
    String string;
    CZ_DEFER(string.drop());
    string.append("abc");
    string.append("defghijklmnopqrstuvwxyz0123456789");
    REQUIRE(string == "abcdefghijklmnopqrstuvwxyz0123456789");
}

TEST_CASE("String::append no realloc") {
    char buffer[64];
    test::MockAllocator test = {buffer, NULL, 0, 64};
    with_global_allocator(test, [&]() {
        String string;
        string.reserve(64);
        string.append("abc");
        string.append("defghijklmnopqrstuvwxyz0123456789");
        REQUIRE(string == "abcdefghijklmnopqrstuvwxyz0123456789");
    });
}

TEST_CASE("String::reserve allocates") {
    char buffer[64];
    test::MockAllocator test = {buffer, NULL, 0, 64};
    with_global_allocator(test, [&]() {
        String string;
        string.reserve(64);
    });
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
