#pragma once

#include "assert.hpp"
#include "context.hpp"
#include "vector_base.hpp"

namespace cz {

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
class alignas(T) Array : public BaseArray<T> {
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
