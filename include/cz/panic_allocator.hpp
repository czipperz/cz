#pragma once

#include "allocator.hpp"
#include "assert.hpp"

namespace cz {

inline void* panic_allocator_realloc(void*, MemSlice old_mem, AllocInfo new_info) {
    CZ_PANIC("cz::panic_allocator called");
}
inline void panic_allocator_dealloc(void*, MemSlice old_mem) {
    // Ignore deallocations.
}

inline Allocator panic_allocator() {
    return {panic_allocator_realloc, panic_allocator_dealloc, nullptr};
}

}
