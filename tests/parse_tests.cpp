#include <czt/test_base.hpp>

#include <cz/parse.hpp>

using namespace cz;

TEST_CASE("parse unsigned") {
    uint8_t u8;
    CHECK(parse("0", &u8) == 1);
    CHECK(u8 == 0);
    CHECK(parse("128", &u8) == 3);
    CHECK(u8 == 128);
    CHECK(parse("255", &u8) == 3);
    CHECK(u8 == 255);

    uint16_t u16;
    CHECK(parse("0", &u16) == 1);
    CHECK(u16 == 0);
    CHECK(parse("128", &u16) == 3);
    CHECK(u16 == 128);
    CHECK(parse("65535", &u16) == 5);
    CHECK(u16 == 65535);

    uint32_t u32;
    CHECK(parse("0", &u32) == 1);
    CHECK(u32 == 0);
    CHECK(parse("128", &u32) == 3);
    CHECK(u32 == 128);
    CHECK(parse("4294967295", &u32) == 10);
    CHECK(u32 == 4294967295);

    uint64_t u64;
    CHECK(parse("0", &u64) == 1);
    CHECK(u64 == 0);
    CHECK(parse("128", &u64) == 3);
    CHECK(u64 == 128);
    CHECK(parse("18446744073709551615", &u64) == 20);
    CHECK(u64 == 18446744073709551615);
}

TEST_CASE("parse signed") {
    int8_t i8;
    CHECK(parse("0", &i8) == 1);
    CHECK(i8 == 0);
    CHECK(parse("64", &i8) == 2);
    CHECK(i8 == 64);
    CHECK(parse("-64", &i8) == 3);
    CHECK(i8 == -64);
    CHECK(parse("128", &i8) == -3);
    CHECK(i8 == -64);
    CHECK(parse("127", &i8) == 3);
    CHECK(i8 == 127);
    CHECK(parse("-128", &i8) == 4);
    CHECK(i8 == -128);
    i8 = -13;
    CHECK(parse("-129", &i8) == -4);
    CHECK(i8 == -13);

    int16_t i16;
    CHECK(parse("0", &i16) == 1);
    CHECK(i16 == 0);
    CHECK(parse("128", &i16) == 3);
    CHECK(i16 == 128);
    CHECK(parse("-128", &i16) == 4);
    CHECK(i16 == -128);
    CHECK(parse("32768", &i16) == -5);
    CHECK(i16 == -128);
    CHECK(parse("32767", &i16) == 5);
    CHECK(i16 == 32767);
    CHECK(parse("-32768", &i16) == 6);
    CHECK(i16 == -32768);
    i16 = 13;
    CHECK(parse("-32769", &i16) == -6);
    CHECK(i16 == 13);

    int32_t i32;
    CHECK(parse("0", &i32) == 1);
    CHECK(i32 == 0);
    CHECK(parse("128", &i32) == 3);
    CHECK(i32 == 128);
    CHECK(parse("-128", &i32) == 4);
    CHECK(i32 == -128);
    CHECK(parse("2147483648", &i32) == -10);
    CHECK(i32 == -128);
    CHECK(parse("2147483647", &i32) == 10);
    CHECK(i32 == 2147483647);
    CHECK(parse("-2147483648", &i32) == 11);
    CHECK(i32 == -2147483648);
    i32 = 13;
    CHECK(parse("-2147483649", &i32) == -11);
    CHECK(i32 == 13);

    int64_t i64;
    CHECK(parse("0", &i64) == 1);
    CHECK(i64 == 0);
    CHECK(parse("128", &i64) == 3);
    CHECK(i64 == 128);
    CHECK(parse("-128", &i64) == 4);
    CHECK(i64 == -128);
    CHECK(parse("9223372036854775808", &i64) == -19);
    CHECK(i64 == -128);
    CHECK(parse("9223372036854775807", &i64) == 19);
    CHECK(i64 == 9223372036854775807);
    CHECK(parse("-9223372036854775808", &i64) == 20);
    CHECK(i64 == -9223372036854775808);
    i64 = 13;
    CHECK(parse("-9223372036854775809", &i64) == -20);
    CHECK(i64 == 13);
}

TEST_CASE("parse bogus makes no progress") {
    int64_t i64;
    CHECK(parse("", &i64) == 0);
    CHECK(parse("hello world", &i64) == 0);
}
