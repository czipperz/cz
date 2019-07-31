#pragma once

#include <stddef.h>

namespace cz {
namespace mem {

struct AllocInfo {
    size_t size;
    size_t alignment;
};

template <class T>
AllocInfo alloc_info() {
    return {
        sizeof(T),
        alignof(T)
    };
}

}
}
