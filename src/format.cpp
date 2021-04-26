#include <cz/format.hpp>

#include <stdio.h>

namespace cz {

// Start of ... wrappers for va_list versions

cz::String asprintf(cz::Allocator allocator, const char* format, ...) {
    va_list args;
    va_start(args, format);
    cz::String string = {};
    append_vsprintf(allocator, &string, format, args);
    va_end(args);
    return string;
}

cz::Heap_String asprintf(const char* format, ...) {
    va_list args;
    va_start(args, format);
    cz::Heap_String string = {};
    append_vsprintf(&string, format, args);
    va_end(args);
    return string;
}

void append_sprintf(cz::Allocator allocator, cz::String* string, const char* format, ...) {
    va_list args;
    va_start(args, format);
    append_vsprintf(allocator, string, format, args);
    va_end(args);
}

void append_sprintf(cz::Heap_String* string, const char* format, ...) {
    va_list args;
    va_start(args, format);
    append_vsprintf(string, format, args);
    va_end(args);
}

// End of ... wrappers for va_list versions

cz::String avsprintf(cz::Allocator allocator, const char* format, va_list args) {
    cz::String string = {};
    append_vsprintf(allocator, &string, format, args);
    return string;
}

cz::Heap_String avsprintf(const char* format, va_list args) {
    cz::Heap_String string = {};
    append_vsprintf(&string, format, args);
    return string;
}

void append_vsprintf(cz::Allocator allocator,
                     cz::String* string,
                     const char* format,
                     va_list args) {
    va_list args2;
    va_copy(args2, args);
    int result = vsnprintf(nullptr, 0, format, args2);
    va_end(args2);
    CZ_DEBUG_ASSERT(result >= 0);

    string->reserve(allocator, (size_t)result + 1);

    int result2 = vsnprintf(string->end(), result + 1, format, args);
    (void)result2;
    CZ_DEBUG_ASSERT(result2 == result);

    string->set_len(string->len() + result);
}

void append_vsprintf(cz::Heap_String* string, const char* format, va_list args) {
    return append_vsprintf(cz::heap_allocator(), string, format, args);
}

void append(cz::Allocator allocator, cz::String* string, short x) {
    append_sprintf(allocator, string, "%hd", x);
}
void append(cz::Allocator allocator, cz::String* string, unsigned short x) {
    append_sprintf(allocator, string, "%hu", x);
}
void append(cz::Allocator allocator, cz::String* string, int x) {
    append_sprintf(allocator, string, "%d", x);
}
void append(cz::Allocator allocator, cz::String* string, unsigned int x) {
    append_sprintf(allocator, string, "%u", x);
}
void append(cz::Allocator allocator, cz::String* string, long x) {
    append_sprintf(allocator, string, "%ld", x);
}
void append(cz::Allocator allocator, cz::String* string, unsigned long x) {
    append_sprintf(allocator, string, "%lu", x);
}
void append(cz::Allocator allocator, cz::String* string, long long x) {
    append_sprintf(allocator, string, "%lld", x);
}
void append(cz::Allocator allocator, cz::String* string, unsigned long long x) {
    append_sprintf(allocator, string, "%llu", x);
}

void append(cz::Allocator allocator, cz::String* string, AllocInfo x) {
    append(allocator, string, "AllocInfo { size: ", x.size, ", alignment: ", x.alignment, " }");
}
void append(cz::Allocator allocator, cz::String* string, MemSlice x) {
    append(allocator, string, "MemSlice { buffer: ", address(x.buffer), ", size: ", x.size, " }");
}

void append(cz::Allocator allocator, cz::String* string, Format_Address address) {
    append_sprintf(allocator, string, "0x%h", address.x);
}

}
