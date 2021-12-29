#include <czt/test_base.hpp>

#include <cz/aligned_buffer.hpp>
#include <cz/arena.hpp>
#include <cz/defer.hpp>
#include <cz/format.hpp>

using namespace cz;

TEST_CASE("format works") {
    Aligned_Buffer<32> buffer;
    Arena arena;
    arena.init(buffer.mem());
    auto string = format(arena.allocator(), 123, " + ", 456);

    REQUIRE(string == "123 + 456");
}

#define FORMAT_NUM_CHECK_AGAINST(TYPE, NUM, STRING)           \
    do {                                                      \
        char buffer[128];                                     \
        Arena arena;                                          \
        arena.init(buffer);                                   \
        auto string = format(arena.allocator(), (TYPE)(NUM)); \
        CHECK(string == STRING);                              \
    } while (0)
#define FORMAT_NUM_CHECK(TYPE, NUM) FORMAT_NUM_CHECK_AGAINST(TYPE, NUM, #NUM);

TEST_CASE("format number limits") {
    FORMAT_NUM_CHECK(int16_t, -32768);
    FORMAT_NUM_CHECK(int16_t, -32767);
    FORMAT_NUM_CHECK(int16_t, 32767);
    FORMAT_NUM_CHECK(uint16_t, 65535);

    FORMAT_NUM_CHECK(int32_t, -2147483648);
    FORMAT_NUM_CHECK(int32_t, -2147483647);
    FORMAT_NUM_CHECK(int32_t, 2147483647);
    FORMAT_NUM_CHECK(uint32_t, 4294967295);

    int64_t s64 = ((int64_t)1 << 63);
    FORMAT_NUM_CHECK_AGAINST(int64_t, s64, "-9223372036854775808");
    s64 += 1;
    FORMAT_NUM_CHECK_AGAINST(int64_t, s64, "-9223372036854775807");
    s64 = -s64;
    FORMAT_NUM_CHECK_AGAINST(int64_t, s64, "9223372036854775807");
    uint64_t u64 = 0;
    u64 = ~u64;
    FORMAT_NUM_CHECK_AGAINST(uint64_t, u64, "18446744073709551615");

#ifdef __SIZEOF_INT128__
    __int128_t s128 = ((__int128_t)1 << 127);
    FORMAT_NUM_CHECK_AGAINST(__int128_t, s128, "-170141183460469231731687303715884105728");
    s128 += 1;
    FORMAT_NUM_CHECK_AGAINST(__int128_t, s128, "-170141183460469231731687303715884105727");
    s128 = -s128;
    FORMAT_NUM_CHECK_AGAINST(__int128_t, s128, "170141183460469231731687303715884105727");
    __uint128_t u128 = 0;
    u128 = ~u128;
    FORMAT_NUM_CHECK_AGAINST(__uint128_t, u128, "340282366920938463463374607431768211455");
#endif
}

TEST_CASE("format number normal") {
    FORMAT_NUM_CHECK(int16_t, 0);
    FORMAT_NUM_CHECK(int16_t, 42);
    FORMAT_NUM_CHECK(int16_t, 1);
    FORMAT_NUM_CHECK(int16_t, -1);
    FORMAT_NUM_CHECK(uint16_t, 0);
    FORMAT_NUM_CHECK(uint16_t, 42);
    FORMAT_NUM_CHECK(uint16_t, 1);
    FORMAT_NUM_CHECK(int32_t, 0);
    FORMAT_NUM_CHECK(int32_t, 42);
    FORMAT_NUM_CHECK(int32_t, 1);
    FORMAT_NUM_CHECK(int32_t, -1);
    FORMAT_NUM_CHECK(uint32_t, 0);
    FORMAT_NUM_CHECK(uint32_t, 42);
    FORMAT_NUM_CHECK(uint32_t, 1);
    FORMAT_NUM_CHECK(int64_t, 0);
    FORMAT_NUM_CHECK(int64_t, 42);
    FORMAT_NUM_CHECK(int64_t, 1);
    FORMAT_NUM_CHECK(int64_t, -1);
    FORMAT_NUM_CHECK(uint64_t, 0);
    FORMAT_NUM_CHECK(uint64_t, 42);
    FORMAT_NUM_CHECK(uint64_t, 1);
#ifdef __SIZEOF_INT128__
    FORMAT_NUM_CHECK(__int128_t, 0);
    FORMAT_NUM_CHECK(__int128_t, 42);
    FORMAT_NUM_CHECK(__int128_t, 1);
    FORMAT_NUM_CHECK(__int128_t, -1);
    FORMAT_NUM_CHECK(__uint128_t, 0);
    FORMAT_NUM_CHECK(__uint128_t, 42);
    FORMAT_NUM_CHECK(__uint128_t, 1);
#endif
}
