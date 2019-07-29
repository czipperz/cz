#include "heap.hpp"

#include <stdlib.h>

namespace cz {
namespace mem {
namespace heap {

Allocator allocator() {
    return {allocate, NULL};
}

void* allocate(void*, void* old_ptr, size_t old_size, size_t new_size) {
    if (old_size == 0) {
        return malloc(new_size);
    } else {
        return ::realloc(old_ptr, new_size);
    }
}

}
}
}
