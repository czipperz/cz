#pragma once

#include <stddef.h>
#include "../src/mem/allocator.hpp"

namespace cz {
namespace mem {
namespace test {

struct MockAllocator {
    void* buffer;
    void* expected_old_ptr;
    size_t expected_old_size;
    AllocInfo expected_new_info;
    bool called = false;

    operator Allocator();
};

}
}
}
