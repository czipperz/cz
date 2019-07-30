#include "string.hpp"

#include <string.h>
#include "assert.hpp"
#include "mem.hpp"

namespace cz {

Str::Str(const char* str) : buffer(str), len(strlen(str)) {}

String::String() : _buffer(NULL), _len(0), _cap(0) {}
String::String(char* buffer, size_t len) : _buffer(buffer), _len(len), _cap(len) {}
String::String(char* buffer, size_t len, size_t cap) : _buffer(buffer), _len(len), _cap(cap) {}

String::String(Str str_to_clone)
    : _buffer(static_cast<char*>(mem::alloc(str_to_clone.len))),
      _len(str_to_clone.len),
      _cap(str_to_clone.len) {
    CZ_ASSERT(_buffer);
    memcpy(_buffer, str_to_clone.buffer, str_to_clone.len);
}

char* String::buffer() {
    return _buffer;
}
const char* String::buffer() const {
    return _buffer;
}
size_t String::len() const {
    return _len;
}
size_t String::cap() const {
    return _cap;
}

Str String::as_str() const {
    return {_buffer, _len};
}

static size_t max(size_t a, size_t b) {
    if (a > b) {
        return a;
    } else {
        return b;
    }
}

void String::reserve(size_t extra) {
    if (_cap - _len < extra) {
        _buffer = static_cast<char*>(mem::realloc(_buffer, _cap, max(_cap + extra, _cap * 2)));
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
