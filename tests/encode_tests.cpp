#include <czt/test_base.hpp>

#include <cz/encode.hpp>

///////////////////////////////////////////////////////////////////////////////
// encode_hex
///////////////////////////////////////////////////////////////////////////////

TEST_CASE("encode_hex empty") {
    cz::String output = {};
    CZ_DEFER(output.drop(cz::heap_allocator()));
    encode_hex("", cz::heap_allocator(), &output);
    CHECK(output == "");
}

TEST_CASE("encode_hex ascii") {
    cz::String output = {};
    CZ_DEFER(output.drop(cz::heap_allocator()));
    encode_hex("abc", cz::heap_allocator(), &output);
    CHECK(output == "616263");
}

TEST_CASE("encode_hex binary") {
    cz::String output = {};
    CZ_DEFER(output.drop(cz::heap_allocator()));
    uint8_t input[] = {0x00, 0xab, 0x12, 0x01, 0x10};
    encode_hex({(const char*)input, sizeof(input)}, cz::heap_allocator(), &output);
    CHECK(output == "00ab120110");
}

///////////////////////////////////////////////////////////////////////////////
// decode_hex
///////////////////////////////////////////////////////////////////////////////

TEST_CASE("decode_hex empty") {
    cz::String output = {};
    CZ_DEFER(output.drop(cz::heap_allocator()));
    decode_hex("", cz::heap_allocator(), &output);
    CHECK(output == "");
}

TEST_CASE("decode_hex ascii") {
    cz::String output = {};
    CZ_DEFER(output.drop(cz::heap_allocator()));
    decode_hex("616263", cz::heap_allocator(), &output);
    CHECK(output == "abc");
}

TEST_CASE("decode_hex binary") {
    cz::String output = {};
    CZ_DEFER(output.drop(cz::heap_allocator()));
    decode_hex("00ab120110", cz::heap_allocator(), &output);
    uint8_t expected[] = {0x00, 0xab, 0x12, 0x01, 0x10};
    CHECK(output.len == sizeof(expected));
    CHECK(memcmp(output.buffer, expected, sizeof(expected)) == 0);
}

TEST_CASE("decode_hex odd length") {
    cz::String output = {};
    CZ_DEFER(output.drop(cz::heap_allocator()));
    decode_hex("abc", cz::heap_allocator(), &output);
    uint8_t expected[] = {0xab, 0xc0};
    CHECK(output.len == sizeof(expected));
    CHECK(memcmp(output.buffer, expected, sizeof(expected)) == 0);
}

///////////////////////////////////////////////////////////////////////////////
// encode_base64
///////////////////////////////////////////////////////////////////////////////

TEST_CASE("encode_base64 empty") {
    cz::String output = {};
    CZ_DEFER(output.drop(cz::heap_allocator()));
    encode_base64("", cz::heap_allocator(), &output);
    CHECK(output == "");
}

TEST_CASE("encode_base64 ascii") {
    cz::String output = {};
    CZ_DEFER(output.drop(cz::heap_allocator()));
    encode_base64("abc", cz::heap_allocator(), &output);
    CHECK(output == "YWJj");
}

TEST_CASE("encode_base64 binary") {
    cz::String output = {};
    CZ_DEFER(output.drop(cz::heap_allocator()));
    uint8_t input[] = {0x00, 0xab, 0x12, 0x01, 0x10};
    encode_base64({(const char*)input, sizeof(input)}, cz::heap_allocator(), &output);
    CHECK(output == "AKsSARA=");
}

TEST_CASE("encode_base64 binary 2") {
    cz::String output = {};
    CZ_DEFER(output.drop(cz::heap_allocator()));
    uint8_t input[] = {0x00, 0xab, 0x12, 0x01};
    encode_base64({(const char*)input, sizeof(input)}, cz::heap_allocator(), &output);
    CHECK(output == "AKsSAQ==");
}

///////////////////////////////////////////////////////////////////////////////
// decode_base64
///////////////////////////////////////////////////////////////////////////////

TEST_CASE("decode_base64 empty") {
    cz::String output = {};
    CZ_DEFER(output.drop(cz::heap_allocator()));
    decode_base64("", cz::heap_allocator(), &output);
    CHECK(output == "");
}

TEST_CASE("decode_base64 ascii") {
    cz::String output = {};
    CZ_DEFER(output.drop(cz::heap_allocator()));
    decode_base64("YWJj", cz::heap_allocator(), &output);
    CHECK(output == "abc");
}

TEST_CASE("decode_base64 binary") {
    cz::String output = {};
    CZ_DEFER(output.drop(cz::heap_allocator()));
    decode_base64("AKsSARA=", cz::heap_allocator(), &output);
    uint8_t expected[] = {0x00, 0xab, 0x12, 0x01, 0x10};
    CHECK(output.len == sizeof(expected));
    CHECK(memcmp(output.buffer, expected, sizeof(expected)) == 0);
}

TEST_CASE("decode_base64 binary 2") {
    cz::String output = {};
    CZ_DEFER(output.drop(cz::heap_allocator()));
    decode_base64("AKsSAQ==", cz::heap_allocator(), &output);
    uint8_t expected[] = {0x00, 0xab, 0x12, 0x01};
    CHECK(output.len == sizeof(expected));
    CHECK(memcmp(output.buffer, expected, sizeof(expected)) == 0);
}

TEST_CASE("decode_base64 binary 3") {
    cz::String output = {};
    CZ_DEFER(output.drop(cz::heap_allocator()));
    decode_base64("AKsSAQ", cz::heap_allocator(), &output);
    uint8_t expected[] = {0x00, 0xab, 0x12, 0x01};
    CHECK(output.len == sizeof(expected));
    CHECK(memcmp(output.buffer, expected, sizeof(expected)) == 0);
}
