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

void append_vsprintf(Allocator allocator,
                     String* string,
                     const char* format,
                     va_list args) {
    append_vsprintf_impl(allocator, string, format, args, false);
}

void append_vsprintf(Heap_String* string, const char* format, va_list args) {
    append_vsprintf_impl(heap_allocator(), string, format, args, false);
}

#define APPEND_NUM(SIGNED, UNSIGNED, MIN)                                  \
    void append(Allocator allocator, String* string, UNSIGNED x) { \
        size_t start = string->len();                                      \
                                                                           \
        while (x >= 10) {                                                  \
            append(allocator, string, '0' + x % 10);                       \
            x /= 10;                                                       \
        }                                                                  \
        CZ_DEBUG_ASSERT(x < 10);                                           \
        append(allocator, string, '0' + x);                                \
                                                                           \
        size_t end = (string->len() - start) / 2;                          \
        for (size_t i = 0; i < end; ++i) {                                 \
            swap((*string)[i], (*string)[string->len() - i - 1]);      \
        }                                                                  \
    }                                                                      \
                                                                           \
    void append(Allocator allocator, String* string, SIGNED x) {   \
        if (~x == 0) {                                                     \
            append(allocator, string, #MIN);                               \
            return;                                                        \
        }                                                                  \
                                                                           \
        if (x < 0) {                                                       \
            append(allocator, string, '-', (UNSIGNED)-x);                  \
        } else {                                                           \
            append(allocator, string, (UNSIGNED)x);                        \
        }                                                                  \
    }

#define SIGNED int16_t
#define UNSIGNED uint16_t
#define MIN -32768
#include "format_num.tpp"

#define SIGNED int32_t
#define UNSIGNED uint32_t
#define MIN -2147483648
#include "format_num.tpp"

#define SIGNED int64_t
#define UNSIGNED uint64_t
#define MIN -9223372036854775808
#include "format_num.tpp"

#ifdef __SIZEOF_INT128__
#define SIGNED __int128_t
#define UNSIGNED __uint128_t
#define MIN -170141183460469231731687303715884105728
#include "format_num.tpp"
#endif

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

}
