#pragma once

#include "list.hpp"
#include "util.hpp"

namespace cz {

template <class T>
class Vector : public List<T> {
protected:
    constexpr Vector(T* elems, size_t len, size_t cap) : List<T>(elems, len, cap) {}
    Vector(const Vector&) = default;
    Vector& operator=(const Vector&) = default;

public:
    bool is_small() const {
        struct SmallVector : public Vector {
            alignas(T) char _buffer[sizeof(T)];
        };
        auto small_buffer =
            static_cast<const void*>(reinterpret_cast<const SmallVector*>(this)->_buffer);
        return small_buffer == static_cast<const void*>(this->elems());
    }

    void reserve(C* c, size_t extra) {
        if (this->cap() - this->len() < extra) {
            size_t new_cap = max(this->len() + extra, this->cap() * 2);

            T* new_elems;
            if (is_small()) {
                new_elems = static_cast<T*>(c->alloc({new_cap * sizeof(T), alignof(T)}).buffer);
            } else {
                new_elems = static_cast<T*>(
                    c->realloc({this->elems(), this->cap()}, {new_cap * sizeof(T), alignof(T)})
                        .buffer);
            }

            CZ_ASSERT(new_elems != NULL);

            if (is_small()) {
                memcpy(new_elems, this->elems(), this->len() * sizeof(T));
            }

            this->_elems = new_elems;
            this->_cap = new_cap;
        }
    }

    void drop(C* c) {
        if (!is_small()) {
            c->dealloc({this->elems(), this->cap() * sizeof(T)});
        }
    }
};

}
