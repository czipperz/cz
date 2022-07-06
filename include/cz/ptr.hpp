#pragma once

#include <stddef.h>

namespace cz {

#define CZ_DIM(ARRAY) (sizeof(ARRAY) / sizeof(*(ARRAY)))

template <class T>
T orelse(T deflt, T otherwise) {
    return deflt ? deflt : otherwise;
}

template <class T>
size_t orelse(T* deflt, T* buffer, size_t otherwise) {
    return deflt ? deflt - buffer : otherwise;
}

}
