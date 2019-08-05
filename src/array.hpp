#pragma once

#include "assert.hpp"
#include "context.hpp"
#include "list_base.hpp"

namespace cz {

template <class T>
struct ArrayRef : public impl::ListBase<T, ArrayRef<T>> {
    template <size_t cap>
    constexpr explicit ArrayRef(T (&buffer)[cap], size_t len)
        : ArrayRef(buffer, len, cap) {}
    constexpr explicit ArrayRef(T* buffer, size_t len, size_t cap)
        : impl::ListBase<T, ArrayRef<T>>(buffer, len, cap) {}

    void reserve(C* c, size_t extra) {
        if (this->cap - this->len < extra) {
            CZ_PANIC(c, "Array length exceeded");
        }
    }

    ArrayRef(const ArrayRef&) = delete;
    ArrayRef& operator=(const ArrayRef&) = delete;
};

template <class T, size_t Capacity>
class Array : public ArrayRef<T> {
    alignas(T) char buffer[sizeof(T) * Capacity];

public:
    constexpr Array() : ArrayRef<T>(reinterpret_cast<T*>(buffer), 0, Capacity) {}

    Array(const Array& other) : Array() { *this = other; }
    Array& operator=(const Array& other) {
        memcpy(buffer, other.buffer, other.len * sizeof(T));
        this->len = other.len;
        return *this;
    }
};

}
