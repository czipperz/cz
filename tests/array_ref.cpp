#include "catch.hpp"

#include "../src/array_ref.hpp"
#include "../src/util.hpp"

using namespace cz;

TEST_CASE("ArrayRef constructor array ref and length") {
    ArrayRef<int> ref;

    CHECK(ref.elems() == NULL);
    CHECK(ref.len() == 0);
    CHECK(ref.cap() == 0);
}

TEST_CASE("ArrayRef contructor array ref and length") {
    int array[5] = {0};
    ArrayRef<int> ref(array, 2);

    CHECK(ref.elems() == array);
    CHECK(ref.len() == 2);
    CHECK(ref.cap() == 5);
}

TEST_CASE("ArrayRef constructor pointer, length, capacity") {
    int array[5] = {0};
    ArrayRef<int> ref(array, 2, 4);

    CHECK(ref.elems() == array);
    CHECK(ref.len() == 2);
    CHECK(ref.cap() == 4);
}

TEST_CASE("ArrayRef move construction") {
    int array[5] = {0};
    ArrayRef<int> ref(array, 2, 4);
    ArrayRef<int> copy(move(ref));

    CHECK(copy.elems() == array);
    CHECK(copy.len() == 2);
    CHECK(copy.cap() == 4);

    CHECK(ref.elems() == array);
    CHECK(ref.len() == 2);
    CHECK(ref.cap() == 4);
}

TEST_CASE("ArrayRef move assignment") {
    int array[5] = {0};
    ArrayRef<int> ref(array, 2, 4);
    ArrayRef<int> copy;

    ArrayRef<int>* ret = &(copy = move(ref));

    CHECK(ret == &copy);

    CHECK(copy.elems() == array);
    CHECK(copy.len() == 2);
    CHECK(copy.cap() == 4);

    CHECK(ref.elems() == array);
    CHECK(ref.len() == 2);
    CHECK(ref.cap() == 4);
}
