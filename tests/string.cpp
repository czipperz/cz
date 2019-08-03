#include "catch.hpp"

#include "../src/assert.hpp"
#include "../src/defer.hpp"
#include "../src/mem.hpp"
#include "../src/string.hpp"
#include "mem/mock_allocate.hpp"

using namespace cz;
using namespace cz::mem;

Allocator panic_allocator() {
    return {[](C* c, void*, MemSlice, AllocInfo) -> void* {
                CZ_PANIC(c, "Allocator cannot be called in this context");
                return NULL;
            },
            NULL};
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
    auto mock = test::mock_alloc(buffer, 3);
    C c = {mock.allocator()};

    String string(&c, "abc");

    REQUIRE(mock.called);
}

TEST_CASE("String::append from empty string") {
    mem::StackArena<32> arena;
    C c = {arena.allocator()};

    String string;
    string.append(&c, "abc");

    REQUIRE(string == "abc");
}

TEST_CASE("String::append from non-empty string and reallocates") {
    mem::StackArena<64> arena;
    C c = {arena.allocator()};

    String string;
    string.append(&c, "abc");
    string.append(&c, "defghijklmnopqrstuvwxyz0123456789");

    REQUIRE(string == "abcdefghijklmnopqrstuvwxyz0123456789");
}

TEST_CASE("String::append no realloc") {
    char buffer[64];
    auto mock = test::mock_alloc(buffer, 64);
    C c = {mock.allocator()};

    String string;
    string.reserve(&c, 64);
    mock.called = false;

    string.append(&c, "abc");
    REQUIRE(!mock.called);
    string.append(&c, "defghijklmnopqrstuvwxyz0123456789");
    REQUIRE(!mock.called);

    REQUIRE(string == "abcdefghijklmnopqrstuvwxyz0123456789");
}

TEST_CASE("String::reserve allocates") {
    char buffer[64];
    auto mock = test::mock_alloc(buffer, 64);
    C c = {mock.allocator()};
    String string;

    string.reserve(&c, 64);

    CHECK(string.buffer() == buffer);
    CHECK(string.len() == 0);
    CHECK(string.cap() == 64);
    REQUIRE(mock.called);
}

TEST_CASE("String::insert empty string") {
    auto mock = test::mock_alloc(NULL, 0);
    C c = {mock.allocator()};
    String string;

    string.insert(&c, 0, "");

    CHECK(string.buffer() == NULL);
    REQUIRE(string == "");
}

TEST_CASE("String::insert into empty string") {
    mem::StackArena<32> arena;
    C c = {arena.allocator()};
    String string;

    string.insert(&c, 0, "abc");

    CHECK(string.buffer() != NULL);
    CHECK(string.len() == 3);
    CHECK(string.cap() >= 3);
    REQUIRE(string == "abc");
}

TEST_CASE("String::insert beginning") {
    char buffer[10] = "xyz";
    String string(buffer, 3, 10);

    string.insert(NULL, 0, "abc");

    CHECK(string.buffer() == buffer);
    CHECK(string.len() == 6);
    CHECK(string.cap() >= 6);
    REQUIRE(string == "abcxyz");
}

TEST_CASE("String::insert middle") {
    char buffer[10] = "xyz";
    String string(buffer, 3, 10);

    string.insert(NULL, 1, "abc");

    CHECK(string.buffer() == buffer);
    CHECK(string.len() == 6);
    CHECK(string.cap() >= 6);
    REQUIRE(string == "xabcyz");
}

TEST_CASE("String::insert end") {
    char buffer[10] = "xyz";
    String string(buffer, 3, 10);

    string.insert(NULL, 3, "abc");

    CHECK(string.buffer() == buffer);
    CHECK(string.len() == 6);
    CHECK(string.cap() >= 6);
    REQUIRE(string == "xyzabc");
}

TEST_CASE("String::insert with resize") {
    StackArena<8> arena;
    C c = {arena.allocator()};
    String string;

    string.insert(&c, 0, "abc");

    CHECK(string.buffer() == arena.mem.buffer);
    REQUIRE(string == "abc");
}

TEST_CASE("String::insert resize boundary") {
    char buffer[10] = "xyz";
    String string(buffer, 3, 6);

    string.insert(NULL, 3, "abc");

    CHECK(string.buffer() == buffer);
    CHECK(string.len() == 6);
    CHECK(string.cap() == 6);
    REQUIRE(string == "xyzabc");
}

TEST_CASE("String::insert into long string") {
    char buffer[128] = "once upoa time in a land far far away";
    String string(buffer, strlen(buffer), 128);

    string.insert(NULL, 8, "n ");

    CHECK(string == "once upon a time in a land far far away");
}

TEST_CASE("String::clear sets len to 0 but doesn't drop") {
    char buffer[3] = "ab";
    String string(buffer, 2, 3);

    string.clear();

    CHECK(string.buffer() == buffer);
    CHECK(string.len() == 0);
    REQUIRE(string.cap() == 3);
}

TEST_CASE("Str==Str same length") {
    char buffer[5] = "abcd";
    Str string(buffer, 4);
    Str str(buffer, 4);

    REQUIRE(string == str);
    REQUIRE(str == string);
}

TEST_CASE("Str==Str different lengths") {
    char buffer[5] = "abcd";
    Str x(buffer, 4);
    Str y(buffer, 3);

    REQUIRE(x != y);
    REQUIRE(y != x);
}

TEST_CASE("Str==Str same different addresses") {
    char buffer1[5] = "abcd", buffer2[5] = "abcd";
    Str x(buffer1, 4);
    Str y(buffer2, 4);

    REQUIRE(x == y);
    REQUIRE(y == x);
}

TEST_CASE("Str==Str completely different") {
    Str x = "abc";
    Str y = "def";

    REQUIRE(x != y);
    REQUIRE(y != x);
}

TEST_CASE("Str<Str same") {
    REQUIRE_FALSE(Str("abc") < "abc");
}

TEST_CASE("Str<Str empty left") {
    REQUIRE(Str("") < "abc");
}

TEST_CASE("Str<Str empty right") {
    REQUIRE_FALSE(Str("abc") < "");
}

TEST_CASE("Str<Str lesser left") {
    REQUIRE(Str("abc") < "b");
}

TEST_CASE("Str<Str lesser right") {
    REQUIRE_FALSE(Str("b") < "abc");
}

TEST_CASE("Str<Str substring on left") {
    REQUIRE(Str("ab") < "abc");
}

TEST_CASE("Str<Str substring on right") {
    REQUIRE_FALSE(Str("abc") < "ab");
}

TEST_CASE("Str<Str both empty") {
    REQUIRE_FALSE(Str("") < "");
}
