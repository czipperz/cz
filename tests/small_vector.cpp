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

    vector.push(3);

    CHECK(vector.is_small());
    CHECK(vector.len() == 1);
    CHECK(vector.cap() == 4);
    CHECK(vector[0] == 3);
}

TEST_CASE("SmallVector push in limits doesn't allocate") {
    SmallVector<int, 4> vector;

    vector.push(3);
    vector.push(4);
    vector.push(5);
    vector.push(6);

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

    vector.push(3);
    vector.push(4);
    vector.push(5);
    vector.push(6);

    REQUIRE_THROWS_AS(vector.push(7), PanicReachedException);
}

TEST_CASE("SmallVector reserve reallocates from small buffer to dynamic buffer when full") {
    SmallVector<int, 4> vector;

    vector.push(3);
    vector.push(4);
    vector.push(5);
    vector.push(6);

    int buffer[8];
    auto mock = mock_alloc(buffer, {8 * sizeof(int), alignof(int)});
    C c = ctxt(mock.allocator());

    vector.reserve(&c, 1);

    REQUIRE(mock.called);
}

TEST_CASE("SmallVector reserve reallocates from small buffer to dynamic buffer when empty") {
    SmallVector<int, 4> vector;

    int buffer[8];
    auto mock = mock_alloc(buffer, {8 * sizeof(int), alignof(int)});
    C c = ctxt(mock.allocator());

    vector.reserve(&c, 5);

    REQUIRE(mock.called);
}

TEST_CASE("SmallVector reserve reallocates from small buffer to dynamic buffer when empty") {
    SmallVector<int, 4> vector;

    int buffer[8];
    auto mock = mock_alloc(buffer, {8 * sizeof(int), alignof(int)});
    C c = ctxt(mock.allocator());

    vector.reserve(&c, 5);

    REQUIRE(mock.called);
}
