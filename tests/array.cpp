#include "test_base.hpp"

#include "../src/array.hpp"

using namespace cz;

TEST_CASE("Array construction")  {
    Array<int, 5> ref;

    CHECK(ref.elems() != nullptr);
    CHECK(ref.len() == 0);
    CHECK(ref.cap() == 5);
}
