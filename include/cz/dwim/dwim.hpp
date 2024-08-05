#pragma once

#include "../buffer_array.hpp"
#include "../heap_vector.hpp"
#include "../str.hpp"

#include <stdio.h>
#include "../defer.hpp"

namespace cz {
namespace dwim {

struct Dwim {
    cz::Heap_Vector<cz::Str> errors;
    MemSlice temp_buffer;
    Buffer_Array buffer_array;

    void init() {
        buffer_array.init();
        temp_buffer = {buffer_array.allocator().alloc({1024, 1}), 1024};
    }

    void drop() {
        errors.drop();
        buffer_array.drop();
    }

    void print_errors() const {
        for (size_t i = 0; i < errors.len; ++i) {
            fwrite(errors[i].buffer, 1, errors[i].len, stderr);
            putc('\n', stderr);
        }
    }

    int exit_code() const { return errors.len > 0; }
};

}
}
