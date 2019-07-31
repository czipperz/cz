#pragma once

#include <stddef.h>
#include "../src/mem/allocator.hpp"

namespace cz {
namespace mem {
namespace test {

struct MockAllocator {
    void* buffer;
    MemSlice expected_old_mem;
    AllocInfo expected_new_info;
    bool called = false;

    MockAllocator(void* buffer, MemSlice expected_old_mem, AllocInfo expected_new_info)
        : buffer(buffer),
          expected_old_mem(expected_old_mem),
          expected_new_info(expected_new_info) {}

    operator Allocator();
};

}
}
}
