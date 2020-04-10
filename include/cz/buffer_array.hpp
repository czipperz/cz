#pragma once

#include <stdint.h>
#include <cz/allocator.hpp>

namespace cz {

struct Buffer_Array {
    static constexpr const size_t buffer_size = 1024;

    char** buffers;
    size_t num_buffers;
    size_t outer;
    char* inner;

    void create();
    void clear();
    void drop();
    Allocator allocator();

    struct Save_Point {
        size_t outer;
        size_t inner;
    };
    Save_Point save() const {
        return {
            outer,
            (size_t)(inner - buffers[outer]),
        };
    }
    void restore(Save_Point sp) {
        outer = sp.outer;
        inner = buffers[outer] + sp.inner;
    }
};

}
