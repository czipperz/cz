#pragma once

#include "../util.hpp"
#include "../defer.hpp"
#include "allocator.hpp"

namespace cz {
namespace mem {

/// The global memory allocator.  Defaults to heap allocating memory.
extern Allocator global_allocator;

template <class F>
void with_global_allocator(Allocator allocator, F f) {
    swap(global_allocator, allocator);
    CZ_DEFER(global_allocator = allocator);
    f();
}

}
}
