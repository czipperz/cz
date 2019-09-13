#pragma once

#include <stddef.h>
#include <cz/allocator.hpp>
#include <cz/array.hpp>

namespace cz {
namespace test {

struct MockAllocate {
    void* buffer;
    MemSlice expected_old_mem;
    AllocInfo expected_new_info;
    bool called = false;

    Allocator allocator();

private:
    MockAllocate(void* buffer, MemSlice expected_old_mem, AllocInfo expected_new_info);

    friend MockAllocate mock_alloc(void* buffer, AllocInfo expected_new_info);
    friend MockAllocate mock_dealloc(MemSlice expected_old_mem);
    friend MockAllocate mock_realloc(void* buffer,
                                     MemSlice expected_old_mem,
                                     AllocInfo expected_new_info);
};

MockAllocate mock_alloc(void* buffer, AllocInfo expected_new_info);
MockAllocate mock_dealloc(MemSlice expected_old_mem);
MockAllocate mock_realloc(void* buffer,
                          MemSlice expected_old_mem,
                          AllocInfo expected_new_info);

struct MockAllocateMultiple {
    Slice<MockAllocate> mocks;
    size_t index = 0;

    MockAllocateMultiple(Slice<MockAllocate> mocks);

    Allocator allocator();
    void verify();
};

Allocator panic_allocator();

Allocator capturing_heap_allocator(List<MemSlice>* mems);
void heap_dealloc_all(Slice<MemSlice> mems);

}
}
