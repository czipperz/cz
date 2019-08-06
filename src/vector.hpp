#pragma once

#include "list.hpp"

namespace cz {

template <class T>
class Vector : public List<T> {
protected:
    Vector(const Vector&) = default;
    Vector& operator=(const Vector&) = default;

public:
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
};

}
