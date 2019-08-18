#include "test_base.hpp"

#include "context.hpp"
#include "cz/defer.hpp"
#include "cz/io/write.hpp"
#include "cz/mem.hpp"
#include "cz/vector.hpp"
#include "mem/mock_allocate.hpp"

using namespace cz;
using namespace cz::test;
using namespace cz::mem;
using namespace cz::io;

TEST_CASE("Vector::drop while small does nothing") {
    SmallVector<int, 0> vec;
    REQUIRE(vec.is_small());
    vec.drop(panic_allocator());
}

TEST_CASE("Vector::drop while large deallocates") {
    Array<MemSlice, 1> mems;
    CZ_DEFER(heap_dealloc_all(mems));

    SmallVector<int, 0> vec;
    vec.reserve(capturing_heap_allocator(&mems), 1);
    REQUIRE(!vec.is_small());

    auto mock = mock_dealloc(mems[0]);

    vec.drop(mock.allocator());

    REQUIRE(mock.called);
}

TEST_CASE("SmallVector create") {
    SmallVector<int, 4> vector;

    CHECK(vector.is_small());
    CHECK(vector.len() == 0);
    REQUIRE(vector.cap() == 4);
}

TEST_CASE("SmallVector<0> in an arena correctly knows when it is small") {
    AlignedBuffer<128> buffer;
    Arena arena;
    arena.mem = buffer;

    auto vector = arena.allocator().alloc<SmallVector<int, 0>>();
    new (vector) SmallVector<int, 0>();
    REQUIRE(vector->is_small());

    vector->reserve(arena.allocator(), 1);
    REQUIRE(!vector->is_small());

    vector->push(5);
    REQUIRE(!vector->is_small());
}

TEST_CASE("SmallVector<1> in an arena correctly knows when it is small") {
    AlignedBuffer<128> buffer;
    Arena arena;
    arena.mem = buffer;

    auto vector = arena.allocator().alloc<SmallVector<int, 1>>();
    new (vector) SmallVector<int, 1>();
    REQUIRE(vector->is_small());

    vector->reserve(arena.allocator(), 1);
    REQUIRE(vector->is_small());

    vector->push(5);
    REQUIRE(vector->is_small());
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

    vector.reserve(mock.allocator(), 1);

    REQUIRE(mock.called);
}

TEST_CASE("SmallVector reserve reallocates from small buffer to dynamic buffer when empty") {
    SmallVector<int, 4> vector;

    int buffer[8];
    auto mock = mock_alloc(buffer, {8 * sizeof(int), alignof(int)});

    vector.reserve(mock.allocator(), 5);

    REQUIRE(mock.called);
}

TEST_CASE("SmallVector move constructor small sets elems pointer correctly") {
    SmallVector<int, 1> original;
    SmallVector<int, 1> copy(move(original));

    CHECK(copy.elems() != original.elems());
    CHECK(copy.is_small());
}

TEST_CASE("SmallVector move constructor small sets elemements correctly") {
    SmallVector<int, 1> original;
    original.push(13);
    SmallVector<int, 1> copy(move(original));

    CHECK(copy[0] == 13);
    CHECK(copy.len() == 1);
    CHECK(copy.is_small());
}

TEST_CASE("SmallVector move constructor large sets elemements copies pointer") {
    SmallVector<int, 1> original;
    CZ_DEFER(original.drop(heap_allocator()));
    original.reserve(heap_allocator(), 2);
    original.push(13);
    original.push(27);

    SmallVector<int, 1> copy(move(original));
    CHECK(original.elems() == copy.elems());
    CHECK(copy[0] == 13);
    CHECK(copy[1] == 27);
    CHECK(copy.len() == 2);
    CHECK(!copy.is_small());
}

TEST_CASE("SmallVector move operator small sets elems pointer correctly") {
    SmallVector<int, 1> original;
    SmallVector<int, 1> copy;
    copy = move(original);

    CHECK(copy.elems() != original.elems());
    CHECK(copy.is_small());
}

TEST_CASE("SmallVector move operator small sets elemements correctly") {
    SmallVector<int, 1> original;
    original.push(13);

    SmallVector<int, 1> copy;
    copy = move(original);

    CHECK(copy[0] == 13);
    CHECK(copy.len() == 1);
    CHECK(copy.is_small());
}

TEST_CASE("SmallVector move operator large sets elemements copies pointer") {
    SmallVector<int, 1> original;
    CZ_DEFER(original.drop(heap_allocator()));
    original.reserve(heap_allocator(), 2);
    original.push(13);
    original.push(27);

    SmallVector<int, 1> copy;
    copy = move(original);
    CHECK(original.elems() == copy.elems());
    CHECK(copy[0] == 13);
    CHECK(copy[1] == 27);
    CHECK(copy.len() == 2);
    CHECK(!copy.is_small());
}
