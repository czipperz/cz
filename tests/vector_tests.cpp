#include <czt/test_base.hpp>

#include <cz/defer.hpp>
#include <cz/heap.hpp>
#include <cz/vector.hpp>
#include <czt/mock_allocate.hpp>

using namespace cz;
using namespace cz::test;

TEST_CASE("Vector constructor makes empty vector no capacity") {
    Vector<int> vec = {};
    REQUIRE(vec.elems() == nullptr);
    REQUIRE(vec.len() == 0);
    REQUIRE(vec.cap() == 0);
}

TEST_CASE("Vector::drop not allocated does nothing") {
    Vector<int> vec = {};
    vec.drop(panic_allocator());
}

TEST_CASE("Vector::drop deallocates") {
    Vector<MemSlice> mems = {};
    mems.reserve(heap_allocator(), 1);
    CZ_DEFER(mems.drop(heap_allocator()));
    CZ_DEFER(heap_dealloc_all(mems));

    Vector<int> vec = {};
    vec.reserve(capturing_heap_allocator(&mems), 1);
    REQUIRE(vec.elems() == mems[0].buffer);

    auto mock = mock_dealloc(mems[0]);

    vec.drop(mock.allocator());

    REQUIRE(mock.called);
}

TEST_CASE("Vector first push works correctly") {
    Vector<int> vector = {};
    auto allocator = heap_allocator();
    CZ_DEFER(vector.drop(allocator));
    vector.reserve(allocator, 4);

    vector.push(3);

    CHECK(vector.len() == 1);
    CHECK(vector.cap() == 4);
    CHECK(vector[0] == 3);
}

TEST_CASE("Vector push in limits") {
    Vector<int> vector = {};
    auto allocator = heap_allocator();
    CZ_DEFER(vector.drop(allocator));
    vector.reserve(allocator, 4);

    vector.push(3);
    vector.push(4);
    vector.push(5);
    vector.push(6);

    CHECK(vector.len() == 4);
    CHECK(vector.cap() == 4);
    CHECK(vector[0] == 3);
    CHECK(vector[1] == 4);
    CHECK(vector[2] == 5);
    CHECK(vector[3] == 6);
}

#ifndef NDEBUG
TEST_CASE("Vector first push outside bounds panics as no reserve") {
    Vector<int> vector = {};
    auto allocator = heap_allocator();
    CZ_DEFER(vector.drop(allocator));
    vector.reserve(allocator, 4);

    vector.push(3);
    vector.push(4);
    vector.push(5);
    vector.push(6);

    REQUIRE_THROWS_AS(vector.push(7), PanicReachedException);
}
#endif

TEST_CASE("Vector reserve calls realloc") {
    Vector<int> vector = {};

    auto allocator = heap_allocator();
    vector.reserve(allocator, 4);
    REQUIRE(vector.cap() == 4);

    int* elems = vector.elems();
    CZ_DEFER(allocator.dealloc({elems, 4 * sizeof(int)}));

    vector.push(3);
    vector.push(4);
    vector.push(5);
    vector.push(6);

    int buffer[8];
    auto mock = mock_realloc(buffer, {elems, 4 * sizeof(int)}, {8 * sizeof(int), alignof(int)});

    vector.reserve(mock.allocator(), 1);

    REQUIRE(mock.called);

    REQUIRE(vector.len() == 4);
    REQUIRE(vector.cap() == 8);
}

TEST_CASE("Vector reserve reallocates while empty") {
    Vector<int> vector = {};
    auto allocator = heap_allocator();
    vector.reserve(allocator, 4);
    REQUIRE(vector.cap() == 4);
    int* elems = vector.elems();
    CZ_DEFER(allocator.dealloc({elems, 4 * sizeof(int)}));

    int buffer[8];
    auto mock =
        mock_realloc(buffer, {vector.elems(), 4 * sizeof(int)}, {8 * sizeof(int), alignof(int)});

    vector.reserve(mock.allocator(), 5);

    REQUIRE(mock.called);
}

TEST_CASE("Vector reserve enough space does nothing") {
    Vector<int> vector = {};
    auto allocator = heap_allocator();
    CZ_DEFER(vector.drop(allocator));
    vector.reserve(allocator, 4);

    vector.reserve(panic_allocator(), 4);
}

TEST_CASE("Vector copy constructor copies pointer") {
    Vector<int> original = {};
    CZ_DEFER(original.drop(heap_allocator()));
    original.reserve(heap_allocator(), 2);

    Vector<int> copy(original);
    CHECK(original.elems() == copy.elems());
}

TEST_CASE("Vector copy operator copies pointer") {
    Vector<int> original = {};
    CZ_DEFER(original.drop(heap_allocator()));
    original.reserve(heap_allocator(), 2);

    Vector<int> copy;
    copy = original;
    CHECK(original.elems() == copy.elems());
}

TEST_CASE("Vector append") {
    Vector<int> vector = {};
    CZ_DEFER(vector.drop(heap_allocator()));
    vector.reserve(heap_allocator(), 4);

    vector.push(1);
    int elems[] = {2, 3};
    vector.append(elems);

    REQUIRE(vector.len() == 3);
    CHECK(vector[0] == 1);
    CHECK(vector[1] == 2);
    CHECK(vector[2] == 3);
}
