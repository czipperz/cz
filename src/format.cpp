#include <cz/format.hpp>

#include <stdio.h>

namespace cz {

// Start of ... wrappers for va_list versions

String asprintf(Allocator allocator, const char* format, ...) {
    va_list args;
    va_start(args, format);
    String string = {};
    append_vsprintf(allocator, &string, format, args);
    va_end(args);
    string.realloc_null_terminate(allocator);
    return string;
}

Heap_String asprintf(const char* format, ...) {
    va_list args;
    va_start(args, format);
    Heap_String string = {};
    append_vsprintf(&string, format, args);
    va_end(args);
    string.realloc_null_terminate();
    return string;
}

void append_sprintf(Allocator allocator, String* string, const char* format, ...) {
    va_list args;
    va_start(args, format);
    append_vsprintf(allocator, string, format, args);
    va_end(args);
}

void append_sprintf(Heap_String* string, const char* format, ...) {
    va_list args;
    va_start(args, format);
    append_vsprintf(string, format, args);
    va_end(args);
}

// End of ... wrappers for va_list versions

static void append_vsprintf_impl(Allocator allocator,
                                 String* string,
                                 const char* format,
                                 va_list args,
                                 bool exact) {
    va_list args2;
    va_copy(args2, args);
    int result = vsnprintf(nullptr, 0, format, args2);
    va_end(args2);
    CZ_DEBUG_ASSERT(result >= 0);

    if (exact) {
        string->reserve_exact(allocator, (size_t)result + 1);
    } else {
        string->reserve(allocator, (size_t)result + 1);
    }

    int result2 = vsnprintf(string->end(), result + 1, format, args);
    (void)result2;
    CZ_DEBUG_ASSERT(result2 == result);

    string->len += result;
}

String avsprintf(Allocator allocator, const char* format, va_list args) {
    String string = {};
    append_vsprintf_impl(allocator, &string, format, args, true);
    CZ_DEBUG_ASSERT(string.len + 1 == string.cap);
    CZ_DEBUG_ASSERT(*string.end() == '\0');
    return string;
}

Heap_String avsprintf(const char* format, va_list args) {
    Heap_String string = {};
    append_vsprintf_impl(heap_allocator(), &string, format, args, true);
    CZ_DEBUG_ASSERT(string.len + 1 == string.cap);
    CZ_DEBUG_ASSERT(*string.end() == '\0');
    return string;
}

void append_vsprintf(Allocator allocator, String* string, const char* format, va_list args) {
    append_vsprintf_impl(allocator, string, format, args, false);
}

void append_vsprintf(Heap_String* string, const char* format, va_list args) {
    append_vsprintf_impl(heap_allocator(), string, format, args, false);
}

///////////////////////////////////////////////////////////////////////////////

#define SIGNED int16_t
#define UNSIGNED uint16_t
#define MIN ((int16_t)0x8000ull)
#define MINS "-32768"
#include "format_num.tpp"

#define SIGNED int32_t
#define UNSIGNED uint32_t
#define MIN ((int32_t)0x80000000ull)
#define MINS "-2147483648"
#include "format_num.tpp"

#define SIGNED int64_t
#define UNSIGNED uint64_t
#define MIN ((int64_t)0x8000000000000000ull)
#define MINS "-9223372036854775808"
#include "format_num.tpp"

#ifdef __SIZEOF_INT128__
#define SIGNED __int128_t
#define UNSIGNED __uint128_t
#define MIN ((__int128_t)((__uint128_t)0x8000000000000000ull << 64 | 0x8000000000000000ull))
#define MINS "-170141183460469231731687303715884105728"
#include "format_num.tpp"
#endif

void append(Allocator allocator, String* string, short x) {
    if (sizeof(x) == 2)
        return append_num(allocator, string, (int16_t)x);
    else if (sizeof(x) == 4)
        return append_num(allocator, string, (int32_t)x);
    else if (sizeof(x) == 8)
        return append_num(allocator, string, (int64_t)x);
}
void append(Allocator allocator, String* string, unsigned short x) {
    if (sizeof(x) == 2)
        return append_num(allocator, string, (uint16_t)x);
    else if (sizeof(x) == 4)
        return append_num(allocator, string, (uint32_t)x);
    else if (sizeof(x) == 8)
        return append_num(allocator, string, (uint64_t)x);
}

void append(Allocator allocator, String* string, int x) {
    if (sizeof(x) == 2)
        return append_num(allocator, string, (int16_t)x);
    else if (sizeof(x) == 4)
        return append_num(allocator, string, (int32_t)x);
    else if (sizeof(x) == 8)
        return append_num(allocator, string, (int64_t)x);
}
void append(Allocator allocator, String* string, unsigned int x) {
    if (sizeof(x) == 2)
        return append_num(allocator, string, (uint16_t)x);
    else if (sizeof(x) == 4)
        return append_num(allocator, string, (uint32_t)x);
    else if (sizeof(x) == 8)
        return append_num(allocator, string, (uint64_t)x);
}

void append(Allocator allocator, String* string, long x) {
    if (sizeof(x) == 2)
        return append_num(allocator, string, (int16_t)x);
    else if (sizeof(x) == 4)
        return append_num(allocator, string, (int32_t)x);
    else if (sizeof(x) == 8)
        return append_num(allocator, string, (int64_t)x);
}
void append(Allocator allocator, String* string, unsigned long x) {
    if (sizeof(x) == 2)
        return append_num(allocator, string, (uint16_t)x);
    else if (sizeof(x) == 4)
        return append_num(allocator, string, (uint32_t)x);
    else if (sizeof(x) == 8)
        return append_num(allocator, string, (uint64_t)x);
}

void append(Allocator allocator, String* string, long long x) {
    if (sizeof(x) == 2)
        return append_num(allocator, string, (int16_t)x);
    else if (sizeof(x) == 4)
        return append_num(allocator, string, (int32_t)x);
    else if (sizeof(x) == 8)
        return append_num(allocator, string, (int64_t)x);
}
void append(Allocator allocator, String* string, unsigned long long x) {
    if (sizeof(x) == 2)
        return append_num(allocator, string, (uint16_t)x);
    else if (sizeof(x) == 4)
        return append_num(allocator, string, (uint32_t)x);
    else if (sizeof(x) == 8)
        return append_num(allocator, string, (uint64_t)x);
}

///////////////////////////////////////////////////////////////////////////////

void append(Allocator allocator, String* string, AllocInfo x) {
    append(allocator, string, "AllocInfo { size: ", x.size, ", alignment: ", x.alignment, " }");
}
void append(Allocator allocator, String* string, MemSlice x) {
    append(allocator, string, "MemSlice { buffer: ", address(x.buffer), ", size: ", x.size, " }");
}

void append(Allocator allocator, String* string, Format_Address address) {
    append_sprintf(allocator, string, "0x%h", address.x);
}

void append(Allocator allocator, String* string, Format_Many many) {
    string->reserve(allocator, many.count);
    string->push_many(many.ch, many.count);
}

////////////////////////////////////////////////////////////////////////////////
// Debug format string
////////////////////////////////////////////////////////////////////////////////

void append(Allocator allocator, String* string, Format_DebugString dbg) {
    size_t total = 2 /* "" */;
    for (size_t i = 0; i < dbg.str.len; ++i) {
        if (dbg.str[i] == '\\' || dbg.str[i] == '\n' || dbg.str[i] == '"')
            total += 2;
        else
            total += 1;
    }

    string->reserve(allocator, total);
    string->push('"');
    for (size_t i = 0; i < dbg.str.len; ++i) {
        if (dbg.str[i] == '\\')
            string->append("\\\\");
        else if (dbg.str[i] == '\n')
            string->append("\\n");
        else if (dbg.str[i] == '"')
            string->append("\\\"");
        else
            string->push(dbg.str[i]);
    }
    string->push('"');
}

}
