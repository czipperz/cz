#pragma once

#include "slice.hpp"
#include "util.hpp"

namespace cz {

template <class T>
void reverse(cz::Slice<T> slice) {
    for (size_t i = 0; i < slice.len / 2; ++i) {
        cz::swap(slice[i], slice[slice.len - 1 - i]);
    }
}

}
