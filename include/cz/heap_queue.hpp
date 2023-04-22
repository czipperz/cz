#pragma once

#include "heap.hpp"
#include "queue.hpp"

namespace cz {

template <class T>
struct Heap_Queue : Queue<T> {
    void drop() { Queue<T>::drop(cz::heap_allocator()); }
    void reserve(size_t extra) { Queue<T>::reserve(cz::heap_allocator(), extra); }
    void reserve_total(size_t total) { Queue<T>::reserve_total(cz::heap_allocator(), total); }
};

}
