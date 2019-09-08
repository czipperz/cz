#include <czt/test_base.hpp>

#include <cz/utf.hpp>

using namespace cz;

TEST_CASE("utf8::is_valid() empty is true") {
    REQUIRE(utf8::is_valid((const uint8_t*)"", 0));
}

TEST_CASE("utf8::is_valid() ascii is true") {
    REQUIRE(utf8::is_valid((const uint8_t*)u8"abcdef", 6));
}

TEST_CASE("utf8::is_valid() greek letters is true") {
    const char* str = u8"μα";
    REQUIRE(utf8::is_valid((const uint8_t*)str, strlen(str)));
}

TEST_CASE("utf8::is_valid() half a greek letter is false") {
    const char* str = u8"μ";
    REQUIRE_FALSE(utf8::is_valid((const uint8_t*)str, 1));
}

TEST_CASE("utf8::is_valid() 3 byte character") {
    const uint8_t str[] = {0xE0, 0xB1, 0x8A};
    REQUIRE(utf8::is_valid(str, 3));
}

TEST_CASE("utf8::is_valid() invalid 3 byte character") {
    const uint8_t str[] = {0xE0, 0xB1, 0x7F};
    REQUIRE_FALSE(utf8::is_valid(str, 3));
}

TEST_CASE("utf8::is_valid() 4 byte character") {
    const uint8_t str[] = {0xF0, 0x91, 0xAB, 0x85};
    REQUIRE(utf8::is_valid(str, 4));
}

TEST_CASE("utf8::is_valid() 0xFF is false") {
    const uint8_t str[] = {0xFF};
    REQUIRE_FALSE(utf8::is_valid(str, 1));
}

TEST_CASE("utf8::forward() ascii") {
    REQUIRE(utf8::forward((const uint8_t*)u8"a") == 1);
}

TEST_CASE("utf8::backward() ascii") {
    REQUIRE(utf8::backward((const uint8_t*)u8"a" + 1) == 1);
}

TEST_CASE("utf8::forward() greek letter") {
    REQUIRE(utf8::forward((const uint8_t*)u8"μ") == 2);
}

TEST_CASE("utf8::backward() greek letter") {
    REQUIRE(utf8::backward((const uint8_t*)u8"μ" + 2) == 2);
}

TEST_CASE("utf8::forward() 3 byte letter") {
    const uint8_t str[] = {0xE0, 0xB1, 0x8A};
    REQUIRE(utf8::forward(str) == 3);
}

TEST_CASE("utf8::backward() 3 byte letter") {
    const uint8_t str[] = {0xE0, 0xB1, 0x8A};
    REQUIRE(utf8::backward(str + 3) == 3);
}

TEST_CASE("utf8::forward() 4 byte letter") {
    const uint8_t str[] = {0xF0, 0x91, 0xAB, 0x85};
    REQUIRE(utf8::forward(str) == 4);
}

TEST_CASE("utf8::backward() 4 byte letter") {
    const uint8_t str[] = {0xF0, 0x91, 0xAB, 0x85};
    REQUIRE(utf8::backward(str + 4) == 4);
}

TEST_CASE("utf8::to_utf32() ascii") {
    REQUIRE(utf8::to_utf32((const uint8_t*)u8"a") == u8"a"[0]);
}

TEST_CASE("utf8::to_utf32() greek letter") {
    REQUIRE(utf8::to_utf32((const uint8_t*)u8"μ") == 0x03BC);
}

TEST_CASE("utf8::to_utf32() 3 byte letter") {
    const uint8_t str[] = {0xE0, 0xB1, 0x8A};
    REQUIRE(utf8::to_utf32(str) == 0x0C4A);
}

TEST_CASE("utf8::to_utf32() 4 byte letter") {
    const uint8_t str[] = {0xF0, 0x91, 0xAB, 0x85};
    REQUIRE(utf8::to_utf32(str) == 0x11AC5);
}

TEST_CASE("utf32::to_utf8() ascii") {
    uint8_t buffer[4];
    REQUIRE(utf32::to_utf8(u8"a"[0], buffer) == 1);
    REQUIRE(buffer[0] == u8"a"[0]);
}

TEST_CASE("utf32::to_utf8() greek letter") {
    uint8_t buffer[4];
    REQUIRE(utf32::to_utf8(0x03BC, buffer) == 2);
    REQUIRE(buffer[0] == 0xCE);
    REQUIRE(buffer[1] == 0xBC);
}

TEST_CASE("utf32::to_utf8() 3 byte letter") {
    uint8_t buffer[4];
    REQUIRE(utf32::to_utf8(0x0C4A, buffer) == 3);
    REQUIRE(buffer[0] == 0xE0);
    REQUIRE(buffer[1] == 0xB1);
    REQUIRE(buffer[2] == 0x8A);
}

TEST_CASE("utf32::to_utf8() 4 byte letter") {
    uint8_t buffer[4];
    REQUIRE(utf32::to_utf8(0x11AC5, buffer) == 4);
    REQUIRE(buffer[0] == 0xF0);
    REQUIRE(buffer[1] == 0x91);
    REQUIRE(buffer[2] == 0xAB);
    REQUIRE(buffer[3] == 0x85);
}
