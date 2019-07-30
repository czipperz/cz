#pragma once

#include "../util.hpp"
#include "../defer.hpp"
#include "allocator.hpp"

namespace cz {
namespace mem {

/// The global memory allocator.  Defaults to heap allocating memory.
extern Allocator global_allocator;

/// Allocate memory using the global allocator.
void* alloc(size_t size);

/// Deallocate memory allocated using the global allocator.
void dealloc(void* ptr, size_t size);

/// Reallocate memory allocated using the global allocator.
void* realloc(void* old_ptr, size_t old_size, size_t new_size);

template <class F>
void with_global_allocator(Allocator allocator, F f) {
    swap(global_allocator, allocator);
    CZ_DEFER(global_allocator = allocator);
    f();
}

}
}
