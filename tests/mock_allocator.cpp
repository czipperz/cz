#include "mock_allocator.hpp"

#include "catch.hpp"

namespace cz {
namespace mem {
namespace test {

static void* test_realloc(void* _data, void* old_ptr, size_t old_size, size_t new_size) {
    auto data = static_cast<MockAllocator*>(_data);
    REQUIRE(data->expected_old_ptr == old_ptr);
    REQUIRE(data->expected_old_size == old_size);
    REQUIRE(data->expected_new_size == new_size);
    data->called = true;
    return data->buffer;
}

MockAllocator::operator Allocator() {
    return {test_realloc, this};
}

}
}
}
