#pragma once

#include <stddef.h>
#include "allocator.hpp"

namespace cz {
namespace mem {

struct Arena {
    /// Make an \c cz::Arena from the buffer and of the given size.
    Arena(void* buffer, size_t size);

    /// Create an uninitialized Arena.
    Arena();

    /// Free the memory controlled by this \c Arena.
    ///
    /// This calls \c dealloc.
    void drop();

    /// Create an \c cz::Allocator allocating memory in the \c Arena.
    Allocator allocator();

    char* start;
    char* point;
    char* end;
};

}
}
