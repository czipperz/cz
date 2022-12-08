#pragma once

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include "file.hpp"
#include "heap_string.hpp"
#include "string.hpp"

namespace cz {

///////////////////////////////////////////////////////////////////////////////
// print using the format library
///////////////////////////////////////////////////////////////////////////////

template <class... Ts>
void print(FILE* file, Allocator allocator, Ts... ts) {
    String string = {};
    CZ_DEFER(string.drop(allocator));
    append(allocator, &string, ts...);
    fwrite(string.buffer, 1, string.len, file);
}
template <class... Ts>
void print(FILE* file, Ts... ts) {
    print(file, cz::heap_allocator(), ts...);
}
template <class... Ts>
void print(Allocator allocator, Ts... ts) {
    print(stdout, allocator, ts...);
}
template <class... Ts>
void print(Ts... ts) {
    print(stdout, cz::heap_allocator(), ts...);
}

template <class... Ts>
void print(Output_File file, Allocator allocator, Ts... ts) {
    String string = {};
    CZ_DEFER(string.drop(allocator));
    append(allocator, &string, ts...);
    file.write(string);
}
template <class... Ts>
void print(Output_File file, Ts... ts) {
    print(file, cz::heap_allocator(), ts...);
}

///////////////////////////////////////////////////////////////////////////////
// sprintf into a string
///////////////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////////
// format -- Concatenate inputs into a string.
///////////////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////////
// Concatenate and append inputs into a string.
///////////////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////////
// Append implementations for basic types.
///////////////////////////////////////////////////////////////////////////////

inline void append(Allocator allocator, String* string, Str str) {
    string->reserve(allocator, str.len);
    string->append(str);
}
/// Be warned that math operators promote chars to ints which cause this overload to not be chosen!
inline void append(Allocator allocator, String* string, char ch) {
    string->reserve(allocator, 1);
    string->push(ch);
}

void append(Allocator allocator, String* string, short);
void append(Allocator allocator, String* string, unsigned short);
void append(Allocator allocator, String* string, int);
void append(Allocator allocator, String* string, unsigned int);
void append(Allocator allocator, String* string, long);
void append(Allocator allocator, String* string, unsigned long);
void append(Allocator allocator, String* string, long long);
void append(Allocator allocator, String* string, unsigned long long);

#ifdef __SIZEOF_INT128__
void append(Allocator allocator, String* string, __int128_t);
void append(Allocator allocator, String* string, __uint128_t);
#endif

void append(Allocator allocator, String* string, AllocInfo);
void append(Allocator allocator, String* string, MemSlice);

template <class T>
void append(Allocator allocator, String* string, Slice<T> slice) {
    append(allocator, string, '[');
    for (size_t i = 0; i < slice.len; ++i) {
        if (i != 0)
            append(allocator, string, ", ");
        append(allocator, string, slice[i]);
    }
    append(allocator, string, ']');
}

template <class T>
inline void append(Allocator allocator, String* string, Vector<T> vector) {
    return append(allocator, string, vector.as_slice());
}

///////////////////////////////////////////////////////////////////////////////
// Custom formatters.
///////////////////////////////////////////////////////////////////////////////

// clang-format off

struct Format_Address { void* x; };
inline Format_Address address(void* x) { return Format_Address{x}; }
void append(Allocator allocator, String* string, Format_Address);

struct Format_Many { char ch; size_t count; };
inline Format_Many many(char ch, size_t count) { return Format_Many{ch, count}; }
void append(Allocator allocator, String* string, Format_Many);

// clang-format on

}
