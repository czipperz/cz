#include <cz/string.hpp>

#include <string.h>
#include <cz/assert.hpp>
#include <cz/util.hpp>

namespace cz {

void String::drop(Allocator allocator) {
    allocator.dealloc(buffer, cap);
}

static void realloc_new_cap(String* string, Allocator allocator, size_t new_cap) {
    char* new_buffer = allocator.realloc(string->buffer, string->cap, new_cap);
    CZ_ASSERT(new_buffer != nullptr);

    string->buffer = new_buffer;
    string->cap = new_cap;
}

void String::reserve_total(Allocator allocator, size_t total) {
    if (cap < total) {
        size_t new_cap = next_power_of_two(total - 1);
        if (new_cap < 16) {
            new_cap = 16;
        }

        realloc_new_cap(this, allocator, new_cap);
    }
}

void String::reserve_exact_total(Allocator allocator, size_t total) {
    if (cap < total) {
        realloc_new_cap(this, allocator, total);
    }
}

void String::realloc(Allocator allocator, size_t new_cap) {
    char* res = allocator.realloc(buffer, cap, new_cap);
    if (res) {
        buffer = res;
        cap = new_cap;
    }
}

void String::realloc_null_terminate(Allocator allocator) {
    realloc_new_cap(this, allocator, len + 1);
    buffer[len] = '\0';
}

void String::push(char ch) {
    CZ_DEBUG_ASSERT(cap - len >= 1);
    buffer[len++] = ch;
}

void String::push_many(char ch, size_t count) {
    CZ_DEBUG_ASSERT(cap - len >= count);
    memset(buffer + len, ch, count);
    len += count;
}

void String::append(Str str) {
    CZ_DEBUG_ASSERT(cap - len >= str.len);
    memcpy(buffer + len, str.buffer, str.len);
    len += str.len;
}

void String::null_terminate() {
    CZ_DEBUG_ASSERT(cap - len >= 1);
    *end() = '\0';
}

void String::insert(size_t index, Str str) {
    CZ_DEBUG_ASSERT(index <= len);
    CZ_DEBUG_ASSERT(cap - len >= str.len);
    memmove(buffer + index + str.len, buffer + index, len - index);
    memcpy(buffer + index, str.buffer, str.len);
    len += str.len;
}

char String::pop() {
    CZ_DEBUG_ASSERT(len >= 1);
    len--;
    return buffer[len];
}

void String::remove(size_t index) {
    CZ_DEBUG_ASSERT(index < len);
    memmove(buffer + index, buffer + index + 1, len - index - 1);
    len -= 1;
}

void String::remove_many(size_t index, size_t count) {
    CZ_DEBUG_ASSERT(index + count <= len);
    memmove(buffer + index, buffer + index + count, len - index - count);
    len -= count;
}

void String::replace(char before, char after) {
    cz::Str str = *this;
    while (1) {
        const char* ptr = str.find(before);
        if (!ptr)
            return;

        *(char*)ptr = after;

        str = str.slice_start(ptr + 1);
    }
}

}
