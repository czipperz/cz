#include <czt/test_base.hpp>

#include <cz/array.hpp>

using namespace cz;

TEST_CASE("ArrayList construction") {
    ArrayList<int, 5> ref;

    CHECK(ref.elems() != nullptr);
    CHECK(ref.len() == 0);
    CHECK(ref.cap() == 5);
}

TEST_CASE("Array construction") {
    Array<int, 5> ref;

    CHECK(ref.elems() != nullptr);
    CHECK(ref.len() == 5);
}
