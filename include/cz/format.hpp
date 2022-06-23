#pragma once

#include <stdarg.h>
#include <stdint.h>
#include "heap_string.hpp"
#include "string.hpp"

namespace cz {

/// Run `sprintf` and make the result into a string; null terminates and truncates the string.
String asprintf(Allocator allocator, const char* format, ...);
Heap_String asprintf(const char* format, ...);
String avsprintf(Allocator allocator, const char* format, va_list args);
Heap_String avsprintf(const char* format, va_list args);

/// Run `sprintf` and append the result to a string.
void append_sprintf(Allocator allocator, String* string, const char* format, ...);
void append_sprintf(Heap_String* string, const char* format, ...);
void append_vsprintf(Allocator allocator, String* string, const char* format, va_list args);
void append_vsprintf(Heap_String* string, const char* format, va_list args);

/// Format a bunch of things to a string; null terminates and truncates the string.
template <class... Ts>
String format(Allocator allocator, Ts... ts) {
    String string = {};
    append(allocator, &string, ts...);
    string.realloc_null_terminate(allocator);
    return string;
}

/// Format a bunch of things to a string that is heap
/// allocated; null terminates and truncates the string.
template <class... Ts>
Heap_String format(Ts... ts) {
    Heap_String string = {};
    append(&string, ts...);
    string.realloc_null_terminate();
    return string;
}

/// Append a bunch of things to a string.
template <class T1, class T2, class... Ts>
void append(Allocator allocator, String* string, T1 t1, T2 t2, Ts... ts) {
    append(allocator, string, t1);
    append(allocator, string, t2, ts...);
}

/// Append a bunch of things to a heap allocated string.
template <class... Ts>
inline void append(Heap_String* string, Ts... ts) {
    append(heap_allocator(), string, ts...);
}

/// Default implementations for basic types.
inline void append(Allocator allocator, String* string, Str str) {
    string->reserve(allocator, str.len);
    string->append(str);
}
/// Be warned that math operators promote chars to ints which cause this overload to not be chosen!
inline void append(Allocator allocator, String* string, char ch) {
    string->reserve(allocator, 1);
    string->push(ch);
}

void append(Allocator allocator, String* string, int16_t);
void append(Allocator allocator, String* string, uint16_t);
void append(Allocator allocator, String* string, int32_t);
void append(Allocator allocator, String* string, uint32_t);
void append(Allocator allocator, String* string, int64_t);
void append(Allocator allocator, String* string, uint64_t);

#if __APPLE__ && __SIZEOF_SIZE_T__ == 8
inline void append(Allocator allocator, String* string, long value) {
    return append(allocator, string, (int64_t)value);
}
inline void append(Allocator allocator, String* string, unsigned long value) {
    return append(allocator, string, (uint64_t)value);
}
#endif

#ifdef __SIZEOF_INT128__
void append(Allocator allocator, String* string, __int128_t);
void append(Allocator allocator, String* string, __uint128_t);
#endif

void append(Allocator allocator, String* string, AllocInfo);
void append(Allocator allocator, String* string, MemSlice);

struct Format_Address {
    void* x;
};
inline Format_Address address(void* x) {
    return Format_Address{x};
}
void append(Allocator allocator, String* string, Format_Address);

struct Format_Many {
    char ch;
    size_t count;
};
inline Format_Many many(char ch, size_t count) {
    return Format_Many{ch, count};
}
void append(Allocator allocator, String* string, Format_Many);

}
