#include "read_to_string.hpp"

#include <stdio.h>
#include "../defer.hpp"
#include "../string.hpp"
#include "../sys.hpp"

namespace cz {
namespace fs {

static bool read(mem::Allocator allocator, String* string, FILE* file, size_t size) {
    string->reserve(allocator, size);
    auto bytes_read = fread(string->buffer() + string->len(), 1, size, file);
    string->set_len(string->len() + bytes_read);
    return bytes_read == size;
}

io::Result read_to_string(mem::Allocator allocator, String* string, FILE* file) {
    const size_t chunk_size = sys::page_size();

    while (true) {
        if (!read(allocator, string, file, chunk_size)) {
            if (feof(file)) {
                break;
            } else {
                return io::Result::last_error();
            }
        }
    }

    return io::Result::ok();
}

io::Result read_to_string(mem::Allocator allocator, String* string, const char* cstr_file_name) {
    auto file = fopen(cstr_file_name, "r");
    if (!file) {
        return io::Result::last_error();
    }
    CZ_DEFER(fclose(file));

    fseek(file, 0, SEEK_END);
    auto len = ftell(file);
    if (len) {
        rewind(file);
        if (!read(allocator, string, file, len)) {
            if (feof(file)) {
                return io::Result::ok();
            } else {
                return io::Result::last_error();
            }
        }
    }

    return read_to_string(allocator, string, file);
}

}
}