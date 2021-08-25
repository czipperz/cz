#include <czt/test_base.hpp>

#include <cz/compare.hpp>

using namespace cz;

TEST_CASE("compare(numbers)") {
    CHECK(compare(-42, 1000) < 0);
    CHECK(compare((int16_t)0x8000, (int16_t)0x7fff) < 0);
    CHECK(compare((int32_t)0x80000000, (int32_t)0x7fffffff) < 0);
}

TEST_CASE("compare(strings)") {
    CHECK(compare(Str("ab"), "de") < 0);
    CHECK(compare(Str("abc"), "de") < 0);
    CHECK(compare(Str("ab"), "abc") < 0);
}

TEST_CASE("lexico(strings)") {
    CHECK(Lexico{}.compare(Str("ab"), "ab") == 0);
    CHECK(Lexico{}.compare(Str("ab"), "de") < 0);
    CHECK(Lexico{}.compare(Str("abc"), "de") < 0);
    CHECK(Lexico{}.compare(Str("ab"), "abc") < 0);
}

TEST_CASE("shortlex(strings)") {
    CHECK(Shortlex{}.compare(Str("ab"), "ab") == 0);
    CHECK(Shortlex{}.compare(Str("ab"), "de") < 0);
    CHECK(Shortlex{}.compare(Str("abc"), "de") > 0);
    CHECK(Shortlex{}.compare(Str("ab"), "abc") < 0);
}

TEST_CASE("lexico(slice ints)") {
    int arr1[] = {1, 2, 3};
    int arr2[] = {4, 5, 6};
    CHECK(Lexico{}.compare(slice(arr1), slice(arr1)) == 0);
    CHECK(Lexico{}.compare(slice(arr1), slice(arr2)) < 0);
    CHECK(Lexico{}.compare(slice(arr1), slice(arr2).slice_end(2)) < 0);
    CHECK(Lexico{}.compare(slice(arr1).slice_end(2), slice(arr1)) < 0);
}

TEST_CASE("shortlex(slice ints)") {
    int arr1[] = {1, 2, 3};
    int arr2[] = {4, 5, 6};
    CHECK(Shortlex{}.compare(slice(arr1), slice(arr1)) == 0);
    CHECK(Shortlex{}.compare(slice(arr1), slice(arr2)) < 0);
    CHECK(Shortlex{}.compare(slice(arr1), slice(arr2).slice_end(2)) > 0);
    CHECK(Shortlex{}.compare(slice(arr1).slice_end(2), slice(arr1)) < 0);
}
