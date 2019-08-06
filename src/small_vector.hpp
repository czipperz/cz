#pragma once

#include "vector.hpp"

namespace cz {

template <class T, size_t BufferSize>
class SmallVector : public Vector<T> {
    alignas(T) char _buffer[sizeof(T) * BufferSize];

public:
    constexpr SmallVector() : Vector<T>(reinterpret_cast<T*>(_buffer), 0, BufferSize) {}

    SmallVector clone(C* c) const {
        auto new_elems = c->alloc({sizeof(T) * this->len, alignof(T)});
        CZ_ASSERT(c, new_elems != NULL);
        memcpy(new_elems, this->elems, sizeof(T) * this->len);
        return {new_elems, this->len, this->len};
    }
};

}
