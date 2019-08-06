#include "string.hpp"

#include <string.h>
#include "assert.hpp"
#include "mem.hpp"
#include "util.hpp"

namespace cz {

String::String() : _buffer(NULL), _len(0), _cap(0) {}
String::String(char* buffer, size_t len, size_t cap) : _buffer(buffer), _len(len), _cap(cap) {}

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

void String::reserve(C* c, size_t extra) {
    if (_cap - _len < extra) {
        size_t new_cap = max(_len + extra, _cap * 2);
        auto new_buffer = static_cast<char*>(c->realloc({_buffer, _cap}, new_cap).buffer);
        CZ_ASSERT(new_buffer != NULL);
        _buffer = new_buffer;
        _cap = new_cap;
    }
}

void String::append(C* c, Str str) {
    reserve(c, str.len);
    memcpy(_buffer + _len, str.buffer, str.len);
    _len += str.len;
    CZ_DEBUG_ASSERT(_len <= _cap);
}

void String::insert(C* c, size_t index, Str str) {
    CZ_ASSERT(index <= _len);
    reserve(c, str.len);
    memmove(_buffer + index + str.len, _buffer + index, len() - index);
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

void String::drop(C* c) {
    c->dealloc({_buffer, _cap});
}

}
