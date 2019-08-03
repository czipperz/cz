#include "mock_allocate.hpp"

#include "../catch.hpp"

namespace cz {
namespace test {

using namespace cz::mem;

MockAllocate::MockAllocate(void* buffer, MemSlice expected_old_mem, AllocInfo expected_new_info)
    : buffer(buffer), expected_old_mem(expected_old_mem), expected_new_info(expected_new_info) {}

static void* test_realloc(C*, void* _data, MemSlice old_mem, AllocInfo new_info) {
    auto data = static_cast<MockAllocate*>(_data);
    CHECK(data->expected_old_mem.buffer == old_mem.buffer);
    CHECK(data->expected_old_mem.size == old_mem.size);
    REQUIRE(data->expected_new_info == new_info);
    data->called = true;
    return data->buffer;
}

Allocator MockAllocate::allocator() {
    return {test_realloc, this};
}

MockAllocate mock_alloc(void* buffer, AllocInfo expected_new_info) {
    return {buffer, {NULL, 0}, expected_new_info};
}
MockAllocate mock_dealloc(MemSlice expected_old_mem) {
    return {NULL, expected_old_mem, {0, 0}};
}
MockAllocate mock_realloc(void* buffer, AllocInfo expected_new_info, MemSlice expected_old_mem) {
    return {buffer, expected_old_mem, expected_new_info};
}

}
}
