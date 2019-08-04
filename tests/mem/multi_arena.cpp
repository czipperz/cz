#include "../catch.hpp"

#include "../../src/defer.hpp"
#include "../../src/mem/heap.hpp"
#include "../../src/mem/multi_arena.hpp"
#include "../../src/vector.hpp"
#include "../context.hpp"
#include "mock_allocate.hpp"

using namespace cz;
using namespace cz::test;
using namespace cz::mem;

TEST_CASE("MultiArena alloc allocates a buffer") {
    char buffer[1024];
    auto mock = mock_alloc(buffer, 1024);
    C c = ctxt(mock.allocator());

    MultiArena multi_arena;
    auto mem = multi_arena.allocator().alloc(&c, 32);

    CHECK(mock.called);
    REQUIRE(mem == MemSlice{buffer + sizeof(MultiArena::Node), 32});
}

TEST_CASE("MultiArena::drop does nothing when no memory") {
    C c = ctxt(panic_allocator());

    MultiArena multi_arena;
    multi_arena.drop(&c);
}

static Allocator capturing_heap_allocator(BaseArray<MemSlice>* mems) {
    return {[](C* c, void* _mems, MemSlice old_mem, AllocInfo new_info) {
                auto mems = static_cast<BaseArray<MemSlice>*>(_mems);
                auto mem = heap_allocator().realloc(NULL, old_mem, new_info);
                REQUIRE(mem.buffer != NULL);
                mems->push(c, mem);
                return mem;
            },
            mems};
}

static void heap_dealloc_all(Slice<MemSlice> mems) {
    for (size_t i = 0; i < mems.len; ++i) {
        heap_allocator().dealloc(NULL, mems[i]);
    }
}

TEST_CASE("MultiArena::drop drops the buffer when there is memory") {
    Array<MemSlice, 1> mems;
    C alloc_context = ctxt(capturing_heap_allocator(&mems));

    MultiArena multi_arena;
    multi_arena.allocator().alloc(&alloc_context, 32);
    REQUIRE(mems.len == 1);
    CZ_DEFER(heap_dealloc_all(mems));

    auto mock = mock_dealloc(mems[0]);
    C drop_context = ctxt(mock.allocator());

    multi_arena.drop(&drop_context);

    REQUIRE(mock.called);
}

TEST_CASE("MultiArena::drop drops all buffers") {
    Array<MemSlice, 2> mems;
    C alloc_context = ctxt(capturing_heap_allocator(&mems));

    MultiArena multi_arena;
    multi_arena.allocator().alloc(&alloc_context, 700);
    multi_arena.allocator().alloc(&alloc_context, 700);
    REQUIRE(mems.len == 2);

    CZ_DEFER(heap_dealloc_all(mems));

    MockAllocate mocks[2] = {mock_dealloc(mems[1]), mock_dealloc(mems[0])};
    MockAllocateMultiple mock(mocks);
    C drop_context = ctxt(mock.allocator());

    multi_arena.drop(&drop_context);

    REQUIRE(mock.index == 2);
}
