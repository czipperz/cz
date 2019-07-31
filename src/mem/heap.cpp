#include "heap.hpp"

#include <stdlib.h>

namespace cz {
namespace mem {
namespace heap {

Allocator allocator() {
    return {allocate, NULL};
}

void* allocate(void*, void* old_ptr, size_t old_size, AllocInfo info) {
    // TODO make alignment work
    if (old_size == 0) {
        return malloc(info.size);
    } else {
        return ::realloc(old_ptr, info.size);
    }
}

}
}
}
