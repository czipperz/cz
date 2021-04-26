#pragma once

#include "heap.hpp"
#include "string.hpp"

namespace cz {

struct Heap_String : String {
    void reserve(size_t extra) { String::reserve(cz::heap_allocator(), extra); }
    void reserve_total(size_t total) { String::reserve_total(cz::heap_allocator(), total); }

    void realloc() { String::realloc(cz::heap_allocator()); }
    void realloc_null_terminate() { String::realloc_null_terminate(cz::heap_allocator()); }

    Heap_String clone() const {
        String scopy = String::clone(cz::heap_allocator());
        Heap_String hcopy;
        hcopy._buffer = scopy._buffer;
        hcopy._len = scopy._len;
        hcopy._cap = scopy._cap;
        return hcopy;
    }
    Heap_String clone_null_terminate() const {
        String scopy = String::clone_null_terminate(cz::heap_allocator());
        Heap_String hcopy;
        hcopy._buffer = scopy._buffer;
        hcopy._len = scopy._len;
        hcopy._cap = scopy._cap;
        return hcopy;
    }

    void drop() { String::drop(cz::heap_allocator()); }
};

}
