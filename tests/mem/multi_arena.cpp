#include "../catch.hpp"

#include "../../src/defer.hpp"
#include "../../src/mem/heap.hpp"
#include "../../src/mem/multi_arena.hpp"
#include "../context.hpp"
#include "mock_allocate.hpp"

using namespace cz;
using namespace cz::test;
using namespace cz::mem;

TEST_CASE("MultiArena alloc allocates a buffer") {
    char buffer[1024];
    auto mock = mock_alloc(buffer, 1024);

    MultiArena multi_arena(mock.allocator());
    auto mem = multi_arena.allocator().alloc(32);

    CHECK(mock.called);
    REQUIRE(mem == MemSlice{buffer + sizeof(MultiArena::Node), 32});
}

TEST_CASE("MultiArena::drop does nothing when no memory") {
    MultiArena multi_arena(panic_allocator());
    multi_arena.drop();
}

TEST_CASE("MultiArena::drop drops the buffer when there is memory") {
    Array<MemSlice, 1> mems;
    CZ_DEFER(heap_dealloc_all(mems));

    MultiArena multi_arena(capturing_heap_allocator(&mems));
    multi_arena.allocator().alloc(32);
    REQUIRE(mems.len() == 1);

    auto mock = mock_dealloc(mems[0]);

    multi_arena.inner_allocator = mock.allocator();
    multi_arena.drop();

    REQUIRE(mock.called);
}

TEST_CASE("MultiArena::drop drops all buffers") {
    Array<MemSlice, 2> mems;
    CZ_DEFER(heap_dealloc_all(mems));

    MultiArena multi_arena(capturing_heap_allocator(&mems));
    multi_arena.allocator().alloc(700);
    multi_arena.allocator().alloc(700);
    REQUIRE(mems.len() == 2);

    MockAllocate mocks[2] = {mock_dealloc(mems[1]), mock_dealloc(mems[0])};
    MockAllocateMultiple mock(mocks);

    multi_arena.inner_allocator = mock.allocator();
    multi_arena.drop();

    REQUIRE(mock.index == 2);
}

TEST_CASE("MultiArena realloc as subset works") {
    Array<MemSlice, 1> mems;
    CZ_DEFER(heap_dealloc_all(mems));

    MultiArena multi_arena(capturing_heap_allocator(&mems));
    auto init = multi_arena.allocator().alloc(32);
    REQUIRE(init.size == 32);

    auto re = multi_arena.allocator().realloc(init, 24);

    REQUIRE(init.buffer == re.buffer);
    REQUIRE(re.size == 24);
    REQUIRE(mems.len() == 1);
}

TEST_CASE("MultiArena realloc works for head") {
    Array<MemSlice, 1> mems;
    CZ_DEFER(heap_dealloc_all(mems));

    MultiArena multi_arena(capturing_heap_allocator(&mems));
    auto init = multi_arena.allocator().alloc(24);
    REQUIRE(init.size == 24);

    auto re = multi_arena.allocator().realloc(init, 32);

    REQUIRE(init.buffer == re.buffer);
    REQUIRE(re.size == 32);
    REQUIRE(mems.len() == 1);
}
