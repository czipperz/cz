#include "allocator.hpp"

namespace cz {
namespace mem {

void* Allocator::alloc(AllocInfo info) {
    return realloc({NULL, 0}, info);
}

void Allocator::dealloc(MemSlice old_mem) {
    realloc(old_mem, {0, 0});
}

void* Allocator::realloc(MemSlice old_mem, AllocInfo new_info) {
    return allocate(data, old_mem, new_info);
}

}
}
