#pragma once

#include "allocator.hpp"

namespace cz {
namespace mem {
namespace heap {
/// Make an allocator that allocates memory in the heap.
Allocator allocator();

/// Allocate memory.  The data parameter is ignored.
void* allocate(void* data, void* old_ptr, size_t old_size, size_t new_size);
}
}
}
