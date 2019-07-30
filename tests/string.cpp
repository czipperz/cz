#include "catch.hpp"

#include "../src/assert.hpp"
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
    REQUIRE(test.called);
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
        REQUIRE(test.called);
    });
}

TEST_CASE("String::reserve allocates") {
    char buffer[64];
    test::MockAllocator test = {buffer, NULL, 0, 64};
    with_global_allocator(test, [&]() {
        String string;
        string.reserve(64);

        REQUIRE(string == "");
        REQUIRE(test.called);
    });
}

TEST_CASE("String::insert empty string") {
    String string;

    string.insert(0, "");

    CHECK(string.buffer() == NULL);
    REQUIRE(string == "");
}

TEST_CASE("String::insert into empty string") {
    String string;
    CZ_DEFER(string.drop());

    string.insert(0, "abc");

    CHECK(string.buffer() != NULL);
    CHECK(string.len() == 3);
    CHECK(string.cap() >= 3);
    REQUIRE(string == "abc");
}

TEST_CASE("String::insert beginning") {
    char buffer[10] = "xyz";
    String string(buffer, 3, 10);

    string.insert(0, "abc");

    CHECK(string.buffer() == buffer);
    CHECK(string.len() == 6);
    CHECK(string.cap() >= 6);
    REQUIRE(string == "abcxyz");
}

TEST_CASE("String::insert middle") {
    char buffer[10] = "xyz";
    String string(buffer, 3, 10);

    string.insert(1, "abc");

    CHECK(string.buffer() == buffer);
    CHECK(string.len() == 6);
    CHECK(string.cap() >= 6);
    REQUIRE(string == "xabcyz");
}

TEST_CASE("String::insert end") {
    char buffer[10] = "xyz";
    String string(buffer, 3, 10);

    string.insert(3, "abc");

    CHECK(string.buffer() == buffer);
    CHECK(string.len() == 6);
    CHECK(string.cap() >= 6);
    REQUIRE(string == "xyzabc");
}

TEST_CASE("String::insert with resize") {
    String string;

    string.insert(0, "abc");

    CZ_DEFER(string.drop());
    CHECK(string.buffer() != NULL);
    REQUIRE(string == "abc");
}

TEST_CASE("String::insert resize boundary") {
    char buffer[10] = "xyz";
    String string(buffer, 3, 6);

    string.insert(3, "abc");

    CHECK(string.buffer() == buffer);
    CHECK(string.len() == 6);
    CHECK(string.cap() == 6);
    REQUIRE(string == "xyzabc");
}

TEST_CASE("String::clear sets len to 0 but doesn't drop") {
    char buffer[3] = "ab";
    String string(buffer, 2, 3);

    Allocator allocator = {[](void*, void*, size_t, size_t) -> void* {
                               CZ_PANIC("called allocator");
                               return NULL;
                           },
                           NULL};
    with_global_allocator(allocator, [&]() { string.clear(); });

    CHECK(string.buffer() == buffer);
    CHECK(string.len() == 0);
    REQUIRE(string.cap() == 3);
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
