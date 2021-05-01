#pragma once

#include "heap.hpp"
#include "vector.hpp"

namespace cz {

template <class T>
struct Heap_Vector : Vector<T> {
    void reserve(size_t extra) { Vector<T>::reserve(cz::heap_allocator(), extra); }
    void reserve_total(size_t total) { Vector<T>::reserve_total(cz::heap_allocator(), total); }

    void drop() { Vector<T>::drop(cz::heap_allocator()); }

    void realloc() { Vector<T>::realloc(cz::heap_allocator()); }

    Heap_Vector<T> clone() const { return clone(cz::heap_allocator()); }
    using Vector::clone;
};

}
