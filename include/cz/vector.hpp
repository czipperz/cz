#pragma once

#include "context.hpp"
#include "list.hpp"
#include "util.hpp"

namespace cz {

template <class T>
struct Vector : List<T> {
    constexpr Vector() : Vector(nullptr, 0, 0) {}
    constexpr Vector(T* elems, size_t len, size_t cap) : List<T>(elems, len, cap) {}
    Vector(const Vector&) = default;
    Vector& operator=(const Vector&) = default;

    void reserve(Allocator allocator, size_t extra) {
        if (this->cap() - this->len() < extra) {
            size_t new_cap = max(this->len() + extra, this->cap() * 2);

            T* new_elems;
            if (this->elems()) {
                new_elems = static_cast<T*>(allocator
                                                .realloc({this->elems(), this->cap() * sizeof(T)},
                                                         {new_cap * sizeof(T), alignof(T)})
                                                .buffer);
            } else {
                new_elems =
                    static_cast<T*>(allocator.alloc({new_cap * sizeof(T), alignof(T)}).buffer);
            }

            CZ_ASSERT(new_elems != nullptr);

            this->_elems = new_elems;
            this->_cap = new_cap;
        }
    }

    void drop(Allocator allocator) {
        if (this->elems()) {
            allocator.dealloc({this->elems(), this->cap() * sizeof(T)});
        }
    }
};

}
