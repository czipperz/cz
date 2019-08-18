#include <cz/string.hpp>

#include <string.h>
#include <cz/assert.hpp>
#include <cz/context.hpp>
#include <cz/mem.hpp>
#include <cz/util.hpp>

namespace cz {

String::String() : _buffer(nullptr), _len(0), _cap(0) {}
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

void String::reserve(mem::Allocator allocator, size_t extra) {
    if (_cap - _len < extra) {
        size_t new_cap = max(_len + extra, _cap * 2);
        char* new_buffer;
        if (_buffer) {
            new_buffer = static_cast<char*>(allocator.realloc({_buffer, _cap}, new_cap).buffer);
        } else {
            new_buffer = static_cast<char*>(allocator.alloc(new_cap).buffer);
        }
        CZ_ASSERT(new_buffer != nullptr);
        _buffer = new_buffer;
        _cap = new_cap;
    }
}

void String::append(Str str) {
    if (_cap - _len < str.len) {
        CZ_PANIC("String::append(): Length exceeded");
    }
    memcpy(_buffer + _len, str.elems, str.len);
    _len += str.len;
    CZ_DEBUG_ASSERT(_len <= _cap);
}

void String::insert_str(size_t index, Str str) {
    CZ_ASSERT(index <= _len);
    if (_cap - _len < str.len) {
        CZ_PANIC("String::insert_str(): Length exceeded");
    }
    memmove(_buffer + index + str.len, _buffer + index, len() - index);
    memcpy(_buffer + index, str.elems, str.len);
    _len += str.len;
    CZ_DEBUG_ASSERT(_len <= _cap);
}

char String::pop() {
    CZ_ASSERT(len() >= 1);
    _len--;
    return _buffer[_len];
}

void String::realloc(mem::Allocator allocator) {
    auto res = allocator.realloc({_buffer, _cap}, {_len, 1}).buffer;
    if (res) {
        _buffer = (char*)res;
        _cap = _len;
    }
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

void String::drop(mem::Allocator allocator) {
    allocator.dealloc({_buffer, _cap});
}

}
