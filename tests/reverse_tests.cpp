#include <czt/test_base.hpp>

#include <cz/reverse.hpp>

TEST_CASE("reverse() 6") {
    int nums[] = {1, 2, 3, 4, 5, 6};
    cz::reverse(cz::slice(nums));
    CHECK(nums[0] == 6);
    CHECK(nums[1] == 5);
    CHECK(nums[2] == 4);
    CHECK(nums[3] == 3);
    CHECK(nums[4] == 2);
    CHECK(nums[5] == 1);
}

TEST_CASE("reverse() 5") {
    int nums[] = {1, 2, 3, 4, 5};
    cz::reverse(cz::slice(nums));
    CHECK(nums[0] == 5);
    CHECK(nums[1] == 4);
    CHECK(nums[2] == 3);
    CHECK(nums[3] == 2);
    CHECK(nums[4] == 1);
}
