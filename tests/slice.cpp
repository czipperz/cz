#include "catch.hpp"

#include "../src/slice.hpp"

TEST_CASE("slice()") {
    auto slice = cz::slice<int>();
    REQUIRE(slice.buffer == NULL);
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
