#include "string.hpp"

#include <string.h>
#include "assert.hpp"
#include "mem.hpp"

namespace cz {

Str::Str(const char* str) : buffer(str), len(strlen(str)) {}

static size_t max(size_t a, size_t b) {
    if (a > b) {
        return a;
    } else {
        return b;
    }
}

void String::reserve(size_t extra) {
    if (_cap - _len < extra) {
        _buffer = static_cast<char*>(
            mem::realloc(_buffer, _cap, max(_cap + extra, _cap * 2)));
        CZ_ASSERT(_buffer != NULL);
    }
}

void String::append(Str str) {
    reserve(str.len);
    memcpy(_buffer + _len, str.buffer, str.len);
    _len += str.len;
    CZ_DEBUG_ASSERT(_len <= _cap);
}

void String::drop() {
    mem::dealloc(_buffer, _cap);
}

bool Str::operator==(const Str& other) const {
    return len == other.len && memcmp(buffer, other.buffer, len) == 0;
}

}
