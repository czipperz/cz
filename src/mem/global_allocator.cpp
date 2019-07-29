#include "global_allocator.hpp"

#include "heap.hpp"

namespace cz {
namespace mem {

Allocator global_allocator = heap::allocator();

void* alloc(size_t size) {
    return global_allocator.alloc(size);
}

void dealloc(void* ptr, size_t size) {
    return global_allocator.dealloc(ptr, size);
}

void* realloc(void* old_ptr, size_t old_size, size_t new_size) {
    return global_allocator.realloc(old_ptr, old_size, new_size);
}

}
}
