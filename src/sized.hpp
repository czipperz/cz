#pragma once

#include <stddef.h>

namespace cz {

struct Sized {
    size_t size;
    size_t alignment;
};

template <class T>
const Sized* sized();

template <>
const Sized* sized<int>();

}
