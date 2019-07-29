#include "allocator.hpp"

#include "heap.hpp"

namespace cz {
namespace mem {

void* alloc(size_t size) {
    return global_allocator.alloc(size);
}

void dealloc(void* ptr, size_t size) {
    return global_allocator.dealloc(ptr, size);
}

void* realloc(void* ptr, size_t size, size_t new_size) {
    return global_allocator.realloc(ptr, size, new_size);
}

void* Allocator::alloc(size_t size) {
    return realloc(NULL, 0, size);
}

void Allocator::dealloc(void* ptr, size_t size) {
    realloc(ptr, size, 0);
}

void* Allocator::realloc(void* old_ptr, size_t old_size, size_t new_size) {
    return allocate(data, old_ptr, old_size, new_size);
}

Allocator global_allocator = heap::allocator();

}
}
