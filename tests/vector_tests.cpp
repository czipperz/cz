#include <czt/test_base.hpp>

#include <cz/defer.hpp>
#include <cz/heap.hpp>
#include <cz/vector.hpp>

using namespace cz;

TEST_CASE("Vector constructor makes empty vector no capacity") {
    Vector<int> vec = {};
    REQUIRE(vec.elems == nullptr);
    REQUIRE(vec.len == 0);
    REQUIRE(vec.cap == 0);
}

TEST_CASE("Vector first push works correctly") {
    Vector<int> vector = {};
    auto allocator = heap_allocator();
    CZ_DEFER(vector.drop(allocator));
    vector.reserve_exact(allocator, 4);

    vector.push(3);

    CHECK(vector.len == 1);
    CHECK(vector.cap == 4);
    CHECK(vector[0] == 3);
}

TEST_CASE("Vector push in limits") {
    Vector<int> vector = {};
    auto allocator = heap_allocator();
    CZ_DEFER(vector.drop(allocator));
    vector.reserve_exact(allocator, 4);

    vector.push(3);
    vector.push(4);
    vector.push(5);
    vector.push(6);

    CHECK(vector.len == 4);
    CHECK(vector.cap == 4);
    CHECK(vector[0] == 3);
    CHECK(vector[1] == 4);
    CHECK(vector[2] == 5);
    CHECK(vector[3] == 6);
}

TEST_CASE("Vector copy constructor copies pointer") {
    Vector<int> original = {};
    CZ_DEFER(original.drop(heap_allocator()));
    original.reserve(heap_allocator(), 2);

    Vector<int> copy(original);
    CHECK(original.elems == copy.elems);
}

TEST_CASE("Vector copy operator copies pointer") {
    Vector<int> original = {};
    CZ_DEFER(original.drop(heap_allocator()));
    original.reserve(heap_allocator(), 2);

    Vector<int> copy;
    copy = original;
    CHECK(original.elems == copy.elems);
}

TEST_CASE("Vector append") {
    Vector<int> vector = {};
    CZ_DEFER(vector.drop(heap_allocator()));
    vector.reserve(heap_allocator(), 4);

    vector.push(1);
    int elems[] = {2, 3};
    vector.append(elems);

    REQUIRE(vector.len == 3);
    CHECK(vector[0] == 1);
    CHECK(vector[1] == 2);
    CHECK(vector[2] == 3);
}
