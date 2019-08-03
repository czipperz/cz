#include "allocator.hpp"

namespace cz {
namespace mem {

void* Allocator::alloc(C* c, AllocInfo info) {
    return realloc(c, {}, info);
}

void Allocator::dealloc(C* c, MemSlice old_mem) {
    realloc(c, old_mem, {});
}

void* Allocator::realloc(C* c, MemSlice old_mem, AllocInfo new_info) {
    return allocate(c, data, old_mem, new_info);
}

}
}
