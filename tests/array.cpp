#include "catch.hpp"

#include "../src/array.hpp"
#include "context.hpp"
#include "mem/mock_allocate.hpp"

using namespace cz;
using namespace cz::test;

TEST_CASE("ArrayRef construction") {
    int array[3];
    ArrayRef<int> ref(array, 0);

    CHECK(ref.elems == array);
    CHECK(ref.len == 0);
    CHECK(ref.cap == 3);
}

TEST_CASE("ArrayRef starting empty can be appended to") {
    C c = ctxt(panic_allocator());

    int array[3] = {0};
    ArrayRef<int> ref(array, 0);
    ref.push(&c, 4);

    CHECK(array[0] == 4);
    CHECK(array[1] == 0);
    CHECK(array[2] == 0);
}

TEST_CASE("ArrayRef starting non-empty can be appended to") {
    C c = ctxt(panic_allocator());

    int array[5] = {3, 4, 5, 6};
    ArrayRef<int> ref(array, 3, 4);
    ref.push(&c, 30);

    CHECK(array[0] == 3);
    CHECK(array[1] == 4);
    CHECK(array[2] == 5);
    CHECK(array[3] == 30);
    CHECK(array[4] == 0);
}

TEST_CASE("ArrayRef insertion at beginning") {
    C c = ctxt(panic_allocator());

    int array[5] = {3, 4, 5, 6};
    ArrayRef<int> ref(array, 3, 4);
    ref.insert(&c, 0, 30);

    CHECK(array[0] == 30);
    CHECK(array[1] == 3);
    CHECK(array[2] == 4);
    CHECK(array[3] == 5);
    CHECK(array[4] == 0);
}

TEST_CASE("ArrayRef insertion in middle") {
    C c = ctxt(panic_allocator());

    int array[5] = {3, 4, 5, 6};
    ArrayRef<int> ref(array, 3, 4);
    ref.insert(&c, 1, 30);

    CHECK(array[0] == 3);
    CHECK(array[1] == 30);
    CHECK(array[2] == 4);
    CHECK(array[3] == 5);
    CHECK(array[4] == 0);
}

TEST_CASE("ArrayRef insertion at end") {
    C c = ctxt(panic_allocator());

    int array[5] = {3, 4, 5, 6};
    ArrayRef<int> ref(array, 3, 4);
    ref.insert(&c, 3, 30);

    CHECK(array[0] == 3);
    CHECK(array[1] == 4);
    CHECK(array[2] == 5);
    CHECK(array[3] == 30);
    CHECK(array[4] == 0);
}
