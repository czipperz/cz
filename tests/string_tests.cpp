#include <czt/test_base.hpp>

#include <cz/aligned_buffer.hpp>
#include <cz/arena.hpp>
#include <cz/assert.hpp>
#include <cz/defer.hpp>
#include <cz/heap.hpp>
#include <cz/string.hpp>

using namespace cz;

TEST_CASE("String::String() is empty") {
    String string = {};

    REQUIRE(string == "");
}

TEST_CASE("String::String(char*, size_t, size_t)") {
    char buffer[4] = "abc";
    String string = {buffer, 2, 4};

    REQUIRE(string == "ab");
    REQUIRE(string.cap == 4);
}

TEST_CASE("String::append from empty string") {
    Aligned_Buffer<32> buffer;
    Arena arena;
    arena.init(buffer.mem());

    String string = {};
    string.reserve(arena.allocator(), 3);
    string.append("abc");

    REQUIRE(string == "abc");
}

TEST_CASE("String::append from non-empty string and reallocates") {
    Aligned_Buffer<64> buffer;
    Arena arena;
    arena.init(buffer.mem());

    String string = {};
    string.reserve(arena.allocator(), 3);
    string.append("abc");
    string.reserve(arena.allocator(), 33);
    string.append("defghijklmnopqrstuvwxyz0123456789");

    REQUIRE(string == "abcdefghijklmnopqrstuvwxyz0123456789");
}

TEST_CASE("String::insert empty string") {
    String string = {};
    string.insert(0, "");

    CHECK(string.buffer == nullptr);
    REQUIRE(string == "");
}

TEST_CASE("String::insert beginning") {
    char buffer[10] = "xyz";
    String string = {buffer, 3, 10};

    string.insert(0, "abc");

    CHECK(string.buffer == buffer);
    CHECK(string.len == 6);
    CHECK(string.cap >= 6);
    REQUIRE(string == "abcxyz");
}

TEST_CASE("String::insert middle") {
    char buffer[10] = "xyz";
    String string = {buffer, 3, 10};

    string.insert(1, "abc");

    CHECK(string.buffer == buffer);
    CHECK(string.len == 6);
    CHECK(string.cap >= 6);
    REQUIRE(string == "xabcyz");
}

TEST_CASE("String::insert end") {
    char buffer[10] = "xyz";
    String string = {buffer, 3, 10};

    string.insert(3, "abc");

    CHECK(string.buffer == buffer);
    CHECK(string.len == 6);
    CHECK(string.cap >= 6);
    REQUIRE(string == "xyzabc");
}

TEST_CASE("String::insert resize boundary") {
    char buffer[10] = "xyz";
    String string = {buffer, 3, 6};

    string.insert(3, "abc");

    CHECK(string.buffer == buffer);
    CHECK(string.len == 6);
    CHECK(string.cap == 6);
    REQUIRE(string == "xyzabc");
}

TEST_CASE("String::insert into long string") {
    char buffer[128] = "once upoa time in a land far far away";
    String string = {buffer, strlen(buffer), 128};

    string.insert(8, "n ");

    CHECK(string == "once upon a time in a land far far away");
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
