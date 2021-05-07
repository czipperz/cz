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

    void resize(size_t new_cap) { Vector<T>::resize(cz::heap_allocator(), new_cap); }

    Heap_Vector<T> clone() const { return clone(cz::heap_allocator()); }
    using Vector<T>::clone;
};

}
