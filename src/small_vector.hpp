#pragma once

#include "vector.hpp"

namespace cz {

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

public:
    constexpr SmallVector() : Vector<T>(reinterpret_cast<T*>(&buffer), 0, BufferSize) {}

    SmallVector(SmallVector&& other) : Vector<T>(0, other.len(), BufferSize) {
        if (other.is_small()) {
            this->_elems = reinterpret_cast<T*>(&buffer);
            memcpy(buffer._buffer, other.buffer._buffer, other.len() * sizeof(T));
        } else {
            this->_elems = other.elems();
        }
    }

    SmallVector& operator=(SmallVector&& other) {
        this->_len = other.len();
        if (other.is_small()) {
            this->_elems = reinterpret_cast<T*>(&buffer);
            memcpy(buffer._buffer, other.buffer._buffer, other.len() * sizeof(T));
        } else {
            this->_elems = other.elems();
        }
        return *this;
    }

    SmallVector clone(C* c) const {
        auto new_elems = c->alloc({sizeof(T) * this->len, alignof(T)});
        CZ_ASSERT(new_elems != NULL);
        memcpy(new_elems, this->elems, sizeof(T) * this->len);
        return {new_elems, this->len, this->len};
    }
};

}
