#include "catch.hpp"

#include "../src/array.hpp"
#include "../src/util.hpp"
#include "context.hpp"
#include "mem/mock_allocate.hpp"

using namespace cz;
using namespace cz::test;

TEST_CASE("ArrayImpl construction") {
    int array[3];
    ArrayImpl<int> ref(array, 0);

    CHECK(ref.elems() == array);
    CHECK(ref.len() == 0);
    CHECK(ref.cap() == 3);
}

TEST_CASE("ArrayImpl starting empty can be appended to") {
    C c = ctxt(panic_allocator());

    int array[3] = {0};
    ArrayImpl<int> ref(array, 0);
    ref.push(&c, 4);

    CHECK(array[0] == 4);
    CHECK(array[1] == 0);
    CHECK(array[2] == 0);
}

TEST_CASE("ArrayImpl starting non-empty can be appended to") {
    C c = ctxt(panic_allocator());

    int array[5] = {3, 4, 5, 6};
    ArrayImpl<int> ref(array, 3, 4);
    ref.push(&c, 30);

    CHECK(array[0] == 3);
    CHECK(array[1] == 4);
    CHECK(array[2] == 5);
    CHECK(array[3] == 30);
    CHECK(array[4] == 0);
}

TEST_CASE("ArrayImpl insertion at beginning") {
    C c = ctxt(panic_allocator());

    int array[5] = {3, 4, 5, 6};
    ArrayImpl<int> ref(array, 3, 4);
    ref.insert(&c, 0, 30);

    CHECK(array[0] == 30);
    CHECK(array[1] == 3);
    CHECK(array[2] == 4);
    CHECK(array[3] == 5);
    CHECK(array[4] == 0);
}

TEST_CASE("ArrayImpl insertion in middle") {
    C c = ctxt(panic_allocator());

    int array[5] = {3, 4, 5, 6};
    ArrayImpl<int> ref(array, 3, 4);
    ref.insert(&c, 1, 30);

    CHECK(array[0] == 3);
    CHECK(array[1] == 30);
    CHECK(array[2] == 4);
    CHECK(array[3] == 5);
    CHECK(array[4] == 0);
}

TEST_CASE("ArrayImpl insertion at end") {
    C c = ctxt(panic_allocator());

    int array[5] = {3, 4, 5, 6};
    ArrayImpl<int> ref(array, 3, 4);
    ref.insert(&c, 3, 30);

    CHECK(array[0] == 3);
    CHECK(array[1] == 4);
    CHECK(array[2] == 5);
    CHECK(array[3] == 30);
    CHECK(array[4] == 0);
}

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
