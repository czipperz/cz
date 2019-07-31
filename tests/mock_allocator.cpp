#include "mock_allocator.hpp"

#include "catch.hpp"

namespace cz {
namespace mem {
namespace test {

static void* test_realloc(void* _data, MemSlice old_mem, AllocInfo new_info) {
    auto data = static_cast<MockAllocator*>(_data);
    CHECK(data->expected_old_mem.buffer == old_mem.buffer);
    CHECK(data->expected_old_mem.len == old_mem.len);
    REQUIRE(data->expected_new_info == new_info);
    data->called = true;
    return data->buffer;
}

MockAllocator::operator Allocator() {
    return {test_realloc, this};
}

}
}
}
