#pragma once

#include "heap.hpp"
#include "string.hpp"

namespace cz {

struct Heap_String : String {
    void reserve(size_t extra) { String::reserve(cz::heap_allocator(), extra); }
    void reserve_total(size_t total) { String::reserve_total(cz::heap_allocator(), total); }

    void reserve_exact(size_t extra) { reserve_exact_total(len + extra); }
    void reserve_exact_total(size_t total) {
        String::reserve_exact_total(cz::heap_allocator(), total);
    }

    void realloc() { String::realloc(cz::heap_allocator()); }
    void realloc_null_terminate() { String::realloc_null_terminate(cz::heap_allocator()); }

    Heap_String clone() const {
        String scopy = String::clone(cz::heap_allocator());
        Heap_String hcopy;
        hcopy.buffer = scopy.buffer;
        hcopy.len = scopy.len;
        hcopy.cap = scopy.cap;
        return hcopy;
    }
    Heap_String clone_null_terminate() const {
        String scopy = String::clone_null_terminate(cz::heap_allocator());
        Heap_String hcopy;
        hcopy.buffer = scopy.buffer;
        hcopy.len = scopy.len;
        hcopy.cap = scopy.cap;
        return hcopy;
    }
    using String::clone;
    using String::clone_null_terminate;

    void drop() { String::drop(cz::heap_allocator()); }
};

}
