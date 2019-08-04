#pragma once

#include "assert.hpp"
#include "context.hpp"
#include "slice.hpp"
#include "util.hpp"

namespace cz {
namespace impl {

template <class T, class Child>
class VectorBase {
public:
    T* elems;
    size_t len;
    size_t cap;

    constexpr VectorBase() : elems(NULL), len(0), cap(0) {}
    constexpr VectorBase(T* elems, size_t len) : elems(elems), len(len), cap(len) {}
    constexpr VectorBase(T* elems, size_t len, size_t cap) : elems(elems), len(len), cap(cap) {}

    void push(C* c, T t) {
        static_cast<Child*>(this)->reserve(c, 1);
        elems[len] = t;
        ++len;
    }

    void insert(C* c, size_t index, T t) {
        CZ_ASSERT(c, index <= len);
        static_cast<Child*>(this)->reserve(c, 1);
        memmove(elems + index + 1, elems + index, (len - index) * sizeof(T));
        elems[index] = t;
        ++len;
    }

    T& operator[](size_t i) { return elems[i]; }
    const T& operator[](size_t i) const { return elems[i]; }

    constexpr operator Slice<T>() const { return {elems, len}; }
};

}

template <class T>
struct Vector : public impl::VectorBase<T, Vector<T>> {
    void reserve(C* c, size_t extra) {
        if (this->cap - this->len < extra) {
            size_t new_cap = max(this->len + extra, this->cap * 2);
            auto new_elems =
                static_cast<T*>(c->realloc({this->elems, this->cap}, {new_cap, alignof(T)}).buffer);
            CZ_ASSERT(c, new_elems != NULL);
            this->elems = new_elems;
            this->cap = new_cap;
        }
    }

    void drop(C* c) { c->dealloc({this->elems, this->cap}); }

    Vector clone(C* c) const {
        auto new_elems = c->alloc({sizeof(T) * this->len, alignof(T)});
        CZ_ASSERT(c, new_elems != NULL);
        memcpy(new_elems, this->elems, sizeof(T) * this->len);
        return {new_elems, this->len, this->len};
    }
};

template <class T>
struct BaseArray : public impl::VectorBase<T, BaseArray<T>> {
    constexpr BaseArray(T* buffer, size_t cap)
        : impl::VectorBase<T, BaseArray<T>>(buffer, 0, cap) {}

    void reserve(C* c, size_t extra) {
        if (this->cap - this->len < extra) {
            CZ_PANIC(c, "Array length exceeded");
        }
    }

    BaseArray(const BaseArray&) = delete;
    BaseArray& operator=(const BaseArray&) = delete;
};

template <class T, size_t static_len>
class Array : public BaseArray<T> {
    char buffer[sizeof(T) * static_len];

public:
    constexpr Array() : BaseArray<T>(reinterpret_cast<T*>(buffer), static_len) {}

    Array(const Array& other) : Array() { *this = other; }
    Array& operator=(const Array& other) {
        memcpy(buffer, other.buffer, other.len * sizeof(T));
        this->len = other.len;
    }
};

}
