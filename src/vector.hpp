#pragma once

#include "assert.hpp"
#include "context.hpp"
#include "util.hpp"
#include "vector_base.hpp"

namespace cz {

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

}
