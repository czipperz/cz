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
    : _buffer(static_cast<char*>(mem::global_allocator.alloc({str_to_clone.len, alignof(char)}))),
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
        size_t new_cap = max(_cap + extra, _cap * 2);
        auto new_buffer = static_cast<char*>(
            mem::global_allocator.realloc({_buffer, _cap}, {new_cap, alignof(char)}));
        CZ_ASSERT(new_buffer != NULL);
        _buffer = new_buffer;
        _cap = new_cap;
    }
}

void String::append(Str str) {
    reserve(str.len);
    memcpy(_buffer + _len, str.buffer, str.len);
    _len += str.len;
    CZ_DEBUG_ASSERT(_len <= _cap);
}

void String::insert(size_t index, Str str) {
    CZ_ASSERT(index <= _len);
    reserve(str.len);
    memmove(_buffer + index + str.len, _buffer + index, str.len);
    memcpy(_buffer + index, str.buffer, str.len);
    _len += str.len;
    CZ_DEBUG_ASSERT(_len <= _cap);
}

void String::clear() {
    set_len(0);
}

void String::shrink_to(size_t new_len) {
    if (new_len <= len()) {
        set_len(new_len);
    } else {
        CZ_PANIC("String::shrink_to(): new_len > String::len()");
    }
}

void String::set_len(size_t new_len) {
    if (new_len <= cap()) {
        _len = new_len;
    } else {
        CZ_PANIC("String::set_len(): new_len > String::cap()");
    }
}

void String::drop() {
    mem::global_allocator.dealloc({_buffer, _cap});
}

bool Str::operator==(const Str& other) const {
    return len == other.len && memcmp(buffer, other.buffer, len) == 0;
}

}
