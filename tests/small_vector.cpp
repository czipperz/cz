#include "catch.hpp"

#include "../src/mem/heap.hpp"
#include "../src/small_vector.hpp"
#include "context.hpp"
#include "mem/mock_allocate.hpp"

using namespace cz;
using namespace cz::test;
using namespace cz::mem;

TEST_CASE("SmallVector create") {
    SmallVector<int, 4> vector;

    CHECK(vector.is_small());
    CHECK(vector.len() == 0);
    REQUIRE(vector.cap() == 4);
}

TEST_CASE("SmallVector first push works correctly") {
    SmallVector<int, 4> vector;

    C c = ctxt(panic_allocator());
    vector.push(&c, 3);

    CHECK(vector.is_small());
    CHECK(vector.len() == 1);
    CHECK(vector.cap() == 4);
    CHECK(vector[0] == 3);
}

TEST_CASE("SmallVector push in limits doesn't allocate") {
    SmallVector<int, 4> vector;

    C c = ctxt(panic_allocator());
    vector.push(&c, 3);
    vector.push(&c, 4);
    vector.push(&c, 5);
    vector.push(&c, 6);

    CHECK(vector.is_small());
    CHECK(vector.len() == 4);
    CHECK(vector.cap() == 4);
    CHECK(vector[0] == 3);
    CHECK(vector[1] == 4);
    CHECK(vector[2] == 5);
    CHECK(vector[3] == 6);
}

TEST_CASE("SmallVector first push outside bounds panics as no reserve") {
    SmallVector<int, 4> vector;

    auto c = ctxt(panic_allocator());
    vector.push(&c, 3);
    vector.push(&c, 4);
    vector.push(&c, 5);
    vector.push(&c, 6);

    c = ctxt(heap_allocator());
    vector.push(&c, 7);
}
