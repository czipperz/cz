#include "catch.hpp"

#include "../src/small_vector.hpp"
#include "context.hpp"
#include "mem/mock_allocate.hpp"

using namespace cz;
using namespace cz::test;

TEST_CASE("Vector::drop while small does nothing") {
    C c = ctxt(panic_allocator());
    SmallVector<int, 0> vec;
    REQUIRE(vec.is_small());
    vec.drop(&c);
}

TEST_CASE("Vector::drop while large deallocates") {
    Array<MemSlice, 1> mems;
    auto c = ctxt(capturing_heap_allocator(&mems));

    SmallVector<int, 0> vec;
    vec.reserve(&c, 1);
    REQUIRE(!vec.is_small());

    auto mock = mock_dealloc(mems[0]);
    c = ctxt(mock.allocator());
    vec.drop(&c);

    REQUIRE(mock.called);
}
