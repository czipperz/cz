#include "allocator.hpp"

namespace cz {
namespace mem {

void* Allocator::alloc(AllocInfo info) {
    return realloc(NULL, 0, info);
}

void Allocator::dealloc(void* ptr, size_t size) {
    realloc(ptr, size, {0, 0});
}

void* Allocator::realloc(void* old_ptr, size_t old_size, AllocInfo info) {
    return allocate(data, old_ptr, old_size, info);
}

}
}
