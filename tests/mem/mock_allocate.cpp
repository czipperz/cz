#include "mock_allocate.hpp"

#include "../../src/mem/heap.hpp"
#include "../catch.hpp"

namespace cz {
namespace test {

using namespace cz::mem;

MockAllocate::MockAllocate(void* buffer, MemSlice expected_old_mem, AllocInfo expected_new_info)
    : buffer(buffer), expected_old_mem(expected_old_mem), expected_new_info(expected_new_info) {}

static MemSlice test_realloc(void* _data, MemSlice old_mem, AllocInfo new_info) {
    auto data = static_cast<MockAllocate*>(_data);
    CHECK(data->expected_old_mem.buffer == old_mem.buffer);
    CHECK(data->expected_old_mem.size == old_mem.size);
    REQUIRE(data->expected_new_info == new_info);
    data->called = true;
    return {data->buffer, new_info.size};
}

static MemSlice test_alloc(void* _data, AllocInfo info) {
    return test_realloc(_data, {NULL, 0}, info);
}

static void test_dealloc(void* _data, MemSlice mem) {
    test_realloc(_data, mem, {0, 0});
}

Allocator MockAllocate::allocator() {
    return {{test_alloc, test_dealloc, test_realloc}, this};
}

MockAllocate mock_alloc(void* buffer, AllocInfo expected_new_info) {
    return {buffer, {NULL, 0}, expected_new_info};
}
MockAllocate mock_dealloc(MemSlice expected_old_mem) {
    return {NULL, expected_old_mem, {0, 0}};
}
MockAllocate mock_realloc(void* buffer, MemSlice expected_old_mem, AllocInfo expected_new_info) {
    return {buffer, expected_old_mem, expected_new_info};
}

static MemSlice panic_alloc(void*, AllocInfo) {
    FAIL("alloc cannot be called in this context");
    return {NULL, 0};
}

static MemSlice panic_realloc(void*, MemSlice, AllocInfo) {
    FAIL("realloc cannot be called in this context");
    return {NULL, 0};
}

static void panic_dealloc(void*, MemSlice) {
    FAIL("dealloc cannot be called in this context");
}

Allocator panic_allocator() {
    return {{panic_alloc, panic_dealloc, panic_realloc}, NULL};
}

MockAllocateMultiple::MockAllocateMultiple(Slice<MockAllocate> mocks) : mocks(mocks) {}

static MemSlice test_multiple_realloc(void* _mocks, MemSlice old_mem, AllocInfo new_info) {
    auto mocks = static_cast<MockAllocateMultiple*>(_mocks);
    REQUIRE(mocks->index < mocks->mocks.len);
    auto mem = mocks->mocks[mocks->index].allocator().realloc(old_mem, new_info);
    ++mocks->index;
    return mem;
}

static MemSlice test_multiple_alloc(void* _mocks, AllocInfo new_info) {
    return test_multiple_realloc(_mocks, {NULL, 0}, new_info);
}

static void test_multiple_dealloc(void* _mocks, MemSlice old_mem) {
    test_multiple_realloc(_mocks, old_mem, {0, 0});
}

mem::Allocator MockAllocateMultiple::allocator() {
    return {{test_multiple_alloc, test_multiple_dealloc, test_multiple_realloc}, this};
}

static MemSlice capturing_heap_realloc(void* _mems, MemSlice old_mem, AllocInfo new_info) {
    auto mems = static_cast<List<MemSlice>*>(_mems);
    auto mem = heap_allocator().realloc(old_mem, new_info);
    REQUIRE(mem.buffer != NULL);
    mems->push(mem);
    return mem;
}

static MemSlice capturing_heap_alloc(void* _mocks, AllocInfo new_info) {
    return capturing_heap_realloc(_mocks, {NULL, 0}, new_info);
}

static void capturing_heap_dealloc(void* _mocks, MemSlice old_mem) {
    capturing_heap_realloc(_mocks, old_mem, {0, 0});
}

Allocator capturing_heap_allocator(List<MemSlice>* mems) {
    return {{capturing_heap_alloc, capturing_heap_dealloc, capturing_heap_realloc}, mems};
}

void heap_dealloc_all(Slice<MemSlice> mems) {
    auto allocator = heap_allocator();
    for (size_t i = 0; i < mems.len; ++i) {
        allocator.dealloc(mems[i]);
    }
}

}
}
