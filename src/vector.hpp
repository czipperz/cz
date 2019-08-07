#pragma once

#include "context.hpp"
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
    constexpr bool is_small() const;

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

namespace impl {
template <class T, size_t BufferSize>
struct SmallVectorBuffer {
    alignas(T) char _buffer[sizeof(T) * BufferSize];
};
template <class T>
struct alignas(T) SmallVectorBuffer<T, 0> {};
}

template <class T, size_t BufferSize>
class SmallVector : public Vector<T> {
    impl::SmallVectorBuffer<T, BufferSize> buffer;

    void assign_move(SmallVector&& other) {
        if (other.is_small()) {
            this->_elems = small_buffer();
            memcpy(buffer._buffer, other.buffer._buffer, other.len() * sizeof(T));
        } else {
            this->_elems = other.elems();
        }
    }

public:
    constexpr SmallVector() : Vector<T>(small_buffer(), 0, BufferSize) {}

    SmallVector(SmallVector&& other) : Vector<T>(0, other.len(), BufferSize) {
        assign_move(move(other));
    }

    SmallVector& operator=(SmallVector&& other) {
        this->_len = other.len();
        assign_move(move(other));
        return *this;
    }

    constexpr const T* small_buffer() const { return reinterpret_cast<const T*>(&buffer); }
    T* small_buffer() { return reinterpret_cast<T*>(&buffer); }

    SmallVector clone(C* c) const {
        auto new_elems = c->alloc({sizeof(T) * this->len, alignof(T)});
        CZ_ASSERT(new_elems != NULL);
        memcpy(new_elems, this->elems, sizeof(T) * this->len);
        return {new_elems, this->len, this->len};
    }
};

template <class T>
constexpr bool Vector<T>::is_small() const {
    return static_cast<const void*>(
               reinterpret_cast<const SmallVector<T, 1>*>(this)->small_buffer()) ==
           static_cast<const void*>(this->elems());
}

}
