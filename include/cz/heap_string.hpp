#pragma once

#include "heap.hpp"
#include "string.hpp"

namespace cz {

struct Heap_String : String {
    void reserve(size_t extra) { String::reserve(cz::heap_allocator(), extra); }
    void reserve_total(size_t total) { String::reserve_total(cz::heap_allocator(), total); }

    void realloc() { String::realloc(cz::heap_allocator()); }
    void realloc_null_terminate() { String::realloc_null_terminate(cz::heap_allocator()); }

    Heap_String clone() const { return as_str().duplicate(cz::heap_allocator()); }
    Heap_String clone_null_terminate() const {
        return as_str().duplicate_null_terminate(cz::heap_allocator());
    }

    void drop() { String::drop(cz::heap_allocator()); }
};

}
