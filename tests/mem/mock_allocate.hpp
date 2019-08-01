#pragma once

#include <stddef.h>
#include "../../src/mem/allocator.hpp"

namespace cz {
namespace mem {
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
                                     AllocInfo expected_new_info,
                                     MemSlice expected_old_mem);
};

MockAllocate mock_alloc(void* buffer, AllocInfo expected_new_info);
MockAllocate mock_dealloc(MemSlice expected_old_mem);
MockAllocate mock_realloc(void* buffer, AllocInfo expected_new_info, MemSlice expected_old_mem);

}
}
}
