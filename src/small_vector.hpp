#pragma once

#include "vector.hpp"

namespace cz {

template <class T, size_t BufferSize>
struct SmallVector : public Vector<T> {
    SmallVector clone(C* c) const {
        auto new_elems = c->alloc({sizeof(T) * this->len, alignof(T)});
        CZ_ASSERT(c, new_elems != NULL);
        memcpy(new_elems, this->elems, sizeof(T) * this->len);
        return {new_elems, this->len, this->len};
    }
};

}
