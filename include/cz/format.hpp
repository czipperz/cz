#pragma once

#include <stdarg.h>
#include <stdint.h>
#include "heap_string.hpp"
#include "string.hpp"

namespace cz {

/// Run `sprintf` and make the result into a string; null terminates and truncates the string.
cz::String asprintf(cz::Allocator allocator, const char* format, ...);
cz::Heap_String asprintf(const char* format, ...);
cz::String avsprintf(cz::Allocator allocator, const char* format, va_list args);
cz::Heap_String avsprintf(const char* format, va_list args);

/// Run `sprintf` and append the result to a string.
void append_sprintf(cz::Allocator allocator, cz::String* string, const char* format, ...);
void append_sprintf(cz::Heap_String* string, const char* format, ...);
void append_vsprintf(cz::Allocator allocator, cz::String* string, const char* format, va_list args);
void append_vsprintf(cz::Heap_String* string, const char* format, va_list args);

/// Format a bunch of things to a string; null terminates and truncates the string.
template <class... Ts>
cz::String format(cz::Allocator allocator, Ts... ts) {
    cz::String string = {};
    append(allocator, &string, ts...);
    string.realloc_null_terminate(allocator);
    return string;
}

/// Format a bunch of things to a string that is heap
/// allocated; null terminates and truncates the string.
template <class... Ts>
cz::Heap_String format(Ts... ts) {
    cz::Heap_String string = {};
    append(&string, ts...);
    string.realloc_null_terminate();
    return string;
}

/// Append a bunch of things to a string.
template <class T1, class T2, class... Ts>
void append(cz::Allocator allocator, cz::String* string, T1 t1, T2 t2, Ts... ts) {
    append(allocator, string, t1);
    append(allocator, string, t2, ts...);
}

/// Append a bunch of things to a heap allocated string.
template <class... Ts>
inline void append(cz::Heap_String* string, Ts... ts) {
    append(cz::heap_allocator(), string, ts...);
}

/// Default implementations for basic types.
inline void append(cz::Allocator allocator, cz::String* string, cz::Str str) {
    string->reserve(allocator, str.len);
    string->append(str);
}
inline void append(cz::Allocator allocator, cz::String* string, char ch) {
    string->reserve(allocator, 1);
    string->push(ch);
}

void append(cz::Allocator allocator, cz::String* string, int16_t);
void append(cz::Allocator allocator, cz::String* string, uint16_t);
void append(cz::Allocator allocator, cz::String* string, int32_t);
void append(cz::Allocator allocator, cz::String* string, uint32_t);
void append(cz::Allocator allocator, cz::String* string, int64_t);
void append(cz::Allocator allocator, cz::String* string, uint64_t);

#ifdef __SIZEOF_INT128__
void append(cz::Allocator allocator, cz::String* string, __int128_t);
void append(cz::Allocator allocator, cz::String* string, __uint128_t);
#endif

void append(cz::Allocator allocator, cz::String* string, AllocInfo);
void append(cz::Allocator allocator, cz::String* string, MemSlice);

struct Format_Address {
    void* x;
};
inline Format_Address address(void* x) {
    return Format_Address{x};
}
void append(cz::Allocator allocator, cz::String* string, Format_Address);

struct Format_Many {
    char ch;
    size_t count;
};
inline Format_Many many(char ch, size_t count) {
    return Format_Many{ch, count};
}
void append(cz::Allocator allocator, cz::String* string, Format_Many);

}
