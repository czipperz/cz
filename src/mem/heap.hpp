#pragma once

#include "../context.hpp"
#include "allocator.hpp"

namespace cz {
namespace mem {

/// Make an allocator that allocates memory in the heap.
Allocator heap_allocator();

}
}
