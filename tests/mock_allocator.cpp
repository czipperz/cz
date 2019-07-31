#include "mock_allocator.hpp"

#include "catch.hpp"

namespace cz {
namespace mem {
namespace test {

static void* test_realloc(void* _data, void* old_ptr, size_t old_size, AllocInfo info) {
    auto data = static_cast<MockAllocator*>(_data);
    CHECK(data->expected_old_ptr == old_ptr);
    CHECK(data->expected_old_size == old_size);
    REQUIRE(data->expected_new_info == info);
    data->called = true;
    return data->buffer;
}

MockAllocator::operator Allocator() {
    return {test_realloc, this};
}

}
}
}
