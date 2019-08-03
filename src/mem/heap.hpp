#pragma once

#include "../context.hpp"
#include "allocator.hpp"

namespace cz {
namespace mem {
namespace heap {

/// Make an allocator that allocates memory in the heap.
Allocator allocator();

/// Allocate memory.  The data parameter is ignored.
void* allocate(C* c, void* _data, MemSlice old_mem, AllocInfo new_info);

}
}
}
