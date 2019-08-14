#include "test_base.hpp"

#include "../src/slice.hpp"

TEST_CASE("slice()") {
    auto slice = cz::slice<int>();
    REQUIRE(slice.buffer == nullptr);
    REQUIRE(slice.len == 0);
}

TEST_CASE("slice(T[])") {
    int buffer[3];
    auto slice = cz::slice(buffer);
    REQUIRE(slice.buffer == buffer);
    REQUIRE(slice.len == 3);
}

TEST_CASE("slice(T*, size_t)") {
    int buffer[3];
    auto slice = cz::slice(buffer, 2);
    REQUIRE(slice.buffer == buffer);
    REQUIRE(slice.len == 2);
}

TEST_CASE("MemSlice()") {
    cz::MemSlice slice;
    REQUIRE(slice.buffer == nullptr);
    REQUIRE(slice.size == 0);
}

TEST_CASE("MemSlice(char[])") {
    char buffer[8];
    cz::MemSlice slice(buffer);
    REQUIRE(slice.buffer == buffer);
    REQUIRE(slice.size == 8);
}

TEST_CASE("MemSlice(nullptr, 0) works") {
    cz::MemSlice slice(nullptr, 0);
    REQUIRE(slice.buffer == nullptr);
    REQUIRE(slice.size == 0);
}

TEST_CASE("MemSlice(char*, size_t)") {
    char buffer[8];
    cz::MemSlice slice(buffer, 6);
    REQUIRE(slice.buffer == buffer);
    REQUIRE(slice.size == 6);
}

TEST_CASE("MemSlice(int*, size_t) acts as buffer size") {
    int buffer[8];
    cz::MemSlice slice(buffer, 6);
    REQUIRE(slice.buffer == buffer);
    REQUIRE(slice.size == 6);
}

TEST_CASE("MemSlice(Slice<int>) acts as logical len") {
    int buffer[8];
    cz::MemSlice slice(cz::slice(buffer));
    REQUIRE(slice.buffer == buffer);
    REQUIRE(slice.size == 8 * sizeof(int));
}

TEST_CASE("MemSlice(void*, size_t)") {
    int buffer[8];
    cz::MemSlice slice((void*)buffer, 6);
    REQUIRE(slice.buffer == buffer);
    REQUIRE(slice.size == 6);
}
