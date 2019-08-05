#include "file.hpp"

#include <stdio.h>
#include "../defer.hpp"
#include "../string.hpp"

namespace cz {
namespace io {

Result read_to_string(C* c, String* string, FILE* file) {
    const size_t chunk_size = 1024;

    while (true) {
        string->reserve(c, chunk_size);
        auto bytes_read = fread(string->buffer() + string->len(), 1, chunk_size, file);
        string->set_len(c, string->len() + bytes_read);

        if (bytes_read < chunk_size) {
            if (feof(file)) {
                break;
            } else {
                return Result::from_errno(c);
            }
        }
    }

    return Result::ok();
}

Result read_to_string(C* c, String* string, const char* cstr_file_name) {
    auto file = fopen(cstr_file_name, "r");
    if (!file) {
        return Result::from_errno(c);
    }
    CZ_DEFER(fclose(file));

    fseek(file, 0, SEEK_END);
    auto len = ftell(file);
    if (len) {
        string->reserve(c, len);
        rewind(file);
        fread(string->buffer(), 1, len, file);
    }

    return read_to_string(c, string, file);
}

}
}
