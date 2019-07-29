#include "allocator.hpp"

namespace cz {
namespace mem {

void* Allocator::alloc(size_t size) {
    return realloc(NULL, 0, size);
}

void Allocator::dealloc(void* ptr, size_t size) {
    realloc(ptr, size, 0);
}

void* Allocator::realloc(void* old_ptr, size_t old_size, size_t new_size) {
    return allocate(data, old_ptr, old_size, new_size);
}

}
}
