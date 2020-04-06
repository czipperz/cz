#include <czt/test_base.hpp>

#include <cz/alloc_info.hpp>

TEST_CASE("alloc_info<char>()") {
    auto info = cz::alloc_info<char>();
    REQUIRE(info.size == sizeof(char));
    REQUIRE(info.alignment == alignof(char));
}

TEST_CASE("alloc_info<int>()") {
    auto info = cz::alloc_info<int>();
    REQUIRE(info.size == sizeof(int));
    REQUIRE(info.alignment == alignof(int));
}

TEST_CASE("AllocInfo(size_t, size_t)") {
    cz::AllocInfo info(4, 2);
    REQUIRE(info.size == 4);
    REQUIRE(info.alignment == 2);
}
