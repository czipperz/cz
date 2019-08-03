#include "../catch.hpp"

#include "../../src/mem/alloc_info.hpp"

TEST_CASE("AllocInfo()") {
    cz::mem::AllocInfo info;
    REQUIRE(info.size == 0);
    REQUIRE(info.alignment == 0);
}

TEST_CASE("alloc_info<char>()") {
    auto info = cz::mem::alloc_info<char>();
    REQUIRE(info.size == sizeof(char));
    REQUIRE(info.alignment == alignof(char));
}

TEST_CASE("alloc_info<int>()") {
    auto info = cz::mem::alloc_info<int>();
    REQUIRE(info.size == sizeof(int));
    REQUIRE(info.alignment == alignof(int));
}

TEST_CASE("AllocInfo(size_t)") {
    cz::mem::AllocInfo info(4);
    REQUIRE(info.size == 4);
    REQUIRE(info.alignment == 1);
}

TEST_CASE("AllocInfo(size_t, size_t)") {
    cz::mem::AllocInfo info(4, 2);
    REQUIRE(info.size == 4);
    REQUIRE(info.alignment == 2);
}
