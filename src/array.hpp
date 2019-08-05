#pragma once

#include "assert.hpp"
#include "context.hpp"
#include "list_base.hpp"

namespace cz {

template <class T>
struct ArrayRef : public impl::ListBase<T, ArrayRef<T>> {
    template <size_t cap>
    constexpr ArrayRef(T (&buffer)[cap], size_t len)
        : ArrayRef(buffer, len, cap) {}
    constexpr ArrayRef(T* buffer, size_t len, size_t cap)
        : impl::ListBase<T, ArrayRef<T>>(buffer, len, cap) {}

    void reserve(C* c, size_t extra) {
        if (this->cap - this->len < extra) {
            CZ_PANIC(c, "Array length exceeded");
        }
    }

    ArrayRef(const ArrayRef&) = delete;
    ArrayRef& operator=(const ArrayRef&) = delete;
};

template <class T, size_t static_len>
class alignas(T) Array : public ArrayRef<T> {
    char buffer[sizeof(T) * static_len];

public:
    constexpr Array() : ArrayRef<T>(reinterpret_cast<T*>(buffer), 0, static_len) {}

    Array(const Array& other) : Array() { *this = other; }
    Array& operator=(const Array& other) {
        memcpy(buffer, other.buffer, other.len * sizeof(T));
        this->len = other.len;
        return *this;
    }
};

}
