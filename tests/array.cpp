#include "catch.hpp"

#include "../src/array.hpp"

using namespace cz;

TEST_CASE("Array construction")  {
    Array<int, 5> ref;

    CHECK(ref.elems() != NULL);
    CHECK(ref.len() == 0);
    CHECK(ref.cap() == 5);
}
