#pragma once

#include "allocator.hpp"

namespace cz {

/// Make an allocator that allocates memory in the heap.
Allocator heap_allocator();

}
