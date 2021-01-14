#pragma once

#include <cz/allocator.hpp>
#include <cz/assert.hpp>
#include <cz/vector.hpp>
#include "next_power_of_two.hpp"

namespace cz {

template <class T>
struct Queue {
    T* elems;
    size_t offset;
    size_t len;
    size_t cap;

    void push(const T& t) {
        CZ_DEBUG_ASSERT(len + 1 <= cap);
        size_t index = (offset + len) & (cap - 1);
        elems[index] = t;
        ++len;
    }

    T pop() {
        CZ_DEBUG_ASSERT(len >= 1);
        --len;
        size_t old_offset = offset;
        offset = (offset + 1) & (cap - 1);
        return elems[old_offset];
    }

    void reserve(cz::Allocator allocator, size_t extra) {
        size_t new_cap = len + extra;
        if (new_cap > cap) {
            CZ_DEBUG_ASSERT(new_cap >= 1);
            new_cap = next_power_of_two(new_cap - 1);

            T* new_elems = static_cast<T*>(
                allocator.realloc({elems, cap * sizeof(T)}, {new_cap * sizeof(T), alignof(T)}));
            CZ_ASSERT(new_elems);

            if (offset + len > cap) {
                size_t overhanging = offset + len - cap;
                size_t underhanging = cap - offset;
                if (overhanging <= underhanging) {
                    // 5__01234 -> ___012345_______
                    memcpy(new_elems + cap, new_elems, sizeof(T) * overhanging);
                } else {
                    // 2345__01 -> 2345__________01
                    memcpy(new_elems + new_cap - underhanging, new_elems + offset,
                           sizeof(T) * underhanging);
                    offset = new_cap - underhanging;
                }
            }

            elems = new_elems;
            cap = new_cap;
        }
    }

    void drop(cz::Allocator allocator) { allocator.dealloc({elems, cap * sizeof(T)}); }

    T& operator[](size_t index) {
        CZ_DEBUG_ASSERT(index < len);
        return elems[(offset + index) & (cap - 1)];
    }
    const T& operator[](size_t index) const {
        CZ_DEBUG_ASSERT(index < len);
        return elems[(offset + index) & (cap - 1)];
    }
};

}
