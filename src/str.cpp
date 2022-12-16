#include <cz/str.hpp>

#include <limits.h>
#include <cz/assert.hpp>
#include <cz/char_type.hpp>
#include <cz/string.hpp>

namespace cz {

String Str::clone(Allocator allocator) const {
    if (len == 0)
        return {};

    auto ptr = static_cast<char*>(allocator.alloc({len, 1}));
    CZ_ASSERT(ptr != nullptr);
    memcpy(ptr, buffer, len);
    return String{ptr, len, len};
}

String Str::clone_null_terminate(Allocator allocator) const {
    auto ptr = static_cast<char*>(allocator.alloc({len + 1, 1}));
    CZ_ASSERT(ptr != nullptr);
    memcpy(ptr, buffer, len);
    ptr[len] = '\0';
    return String{ptr, len, len + 1};
}

static inline bool matches_case_insensitive_unbounded(const char* str, cz::Str piece) {
    for (size_t i = 0; i < piece.len; ++i) {
        if (cz::to_lower(str[i]) != cz::to_lower(piece[i])) {
            return false;
        }
    }
    return true;
}

bool Str::equals_case_insensitive(cz::Str other) const {
    if (other.len != len) {
        return false;
    } else {
        return matches_case_insensitive_unbounded(buffer, other);
    }
}

bool Str::starts_with_case_insensitive(cz::Str prefix) const {
    if (prefix.len > len) {
        return false;
    } else {
        return matches_case_insensitive_unbounded(buffer, prefix);
    }
}

bool Str::ends_with_case_insensitive(cz::Str postfix) const {
    if (postfix.len > len) {
        return false;
    } else {
        return matches_case_insensitive_unbounded(buffer + len - postfix.len, postfix);
    }
}

const char* Str::find(cz::Str infix) const {
#ifdef _GNU_SOURCE
    return static_cast<const char*>(::memmem(buffer, len, infix.buffer, infix.len));
#else
    for (size_t i = 0; i + infix.len <= len; ++i) {
        if (memcmp(buffer + i, infix.buffer, infix.len) == 0) {
            return buffer + i;
        }
    }
    return nullptr;
#endif
}

const char* Str::rfind(cz::Str infix) const {
    if (infix.len > len) {
        return nullptr;
    }
    for (size_t i = len - infix.len + 1; i-- > 0;) {
        if (memcmp(buffer + i, infix.buffer, infix.len) == 0) {
            return buffer + i;
        }
    }
    return nullptr;
}

const char* Str::rfind(char pattern) const {
#ifdef _GNU_SOURCE
    return static_cast<const char*>(::memrchr(buffer, pattern, len));
#else
    return rfind({&pattern, 1});
#endif
}

const char* Str::find_case_insensitive(cz::Str infix) const {
    for (size_t i = 0; i + infix.len <= len; ++i) {
        if (matches_case_insensitive_unbounded(buffer + i, infix)) {
            return buffer + i;
        }
    }
    return nullptr;
}

const char* Str::rfind_case_insensitive(cz::Str infix) const {
    if (infix.len > len) {
        return nullptr;
    }
    for (size_t i = len - infix.len; i > 0; --i) {
        if (matches_case_insensitive_unbounded(buffer + i, infix)) {
            return buffer + i;
        }
    }
    return nullptr;
}

static bool split_excluding_(Str str, const char* sep, size_t len, Str* before, Str* after) {
    if (!sep)
        return false;

    Str ta = str.slice_start(sep + len);
    *before = str.slice_end(sep);
    *after = ta;
    return true;
}

static bool split_before_(Str str, const char* sep, size_t len, Str* before, Str* after) {
    if (!sep)
        return false;

    Str ta = str.slice_start(sep);
    *before = str.slice_end(sep);
    *after = ta;
    return true;
}

static bool split_after_(Str str, const char* sep, size_t len, Str* before, Str* after) {
    if (!sep)
        return false;

    Str ta = str.slice_start(sep + len);
    *before = str.slice_end(sep + len);
    *after = ta;
    return true;
}

bool Str::split_excluding(char separator, Str* before, Str* after) const {
    return split_excluding_(*this, find(separator), 1, before, after);
}
bool Str::split_before(char separator, Str* before, Str* after) const {
    return split_before_(*this, find(separator), 1, before, after);
}
bool Str::split_after(char separator, Str* before, Str* after) const {
    return split_after_(*this, find(separator), 1, before, after);
}

bool Str::split_excluding_last(char separator, Str* before, Str* after) const {
    return split_excluding_(*this, rfind(separator), 1, before, after);
}
bool Str::split_before_last(char separator, Str* before, Str* after) const {
    return split_before_(*this, rfind(separator), 1, before, after);
}
bool Str::split_after_last(char separator, Str* before, Str* after) const {
    return split_after_(*this, rfind(separator), 1, before, after);
}

bool Str::split_excluding(cz::Str separator, Str* before, Str* after) const {
    return split_excluding_(*this, find(separator), separator.len, before, after);
}
bool Str::split_before(cz::Str separator, Str* before, Str* after) const {
    return split_before_(*this, find(separator), separator.len, before, after);
}
bool Str::split_after(cz::Str separator, Str* before, Str* after) const {
    return split_after_(*this, find(separator), separator.len, before, after);
}

bool Str::split_excluding_last(cz::Str separator, Str* before, Str* after) const {
    return split_excluding_(*this, rfind(separator), separator.len, before, after);
}
bool Str::split_before_last(cz::Str separator, Str* before, Str* after) const {
    return split_before_(*this, rfind(separator), separator.len, before, after);
}
bool Str::split_after_last(cz::Str separator, Str* before, Str* after) const {
    return split_after_(*this, rfind(separator), separator.len, before, after);
}

void Str::split_into(char separator, cz::Allocator allocator, cz::Vector<cz::Str>* values) const {
    cz::Str remaining = *this;
    while (1) {
        cz::Str value = remaining;
        bool split = remaining.split_excluding(separator, &value, &remaining);

        values->reserve(allocator, 1);
        values->push(value);

        if (!split)
            break;
    }
}

void Str::split_into(cz::Str separator, cz::Allocator allocator, cz::Vector<cz::Str>* values) const {
    cz::Str remaining = *this;
    while (1) {
        cz::Str value = remaining;
        bool split = remaining.split_excluding(separator, &value, &remaining);

        values->reserve(allocator, 1);
        values->push(value);

        if (!split)
            break;
    }
}

void Str::split_clone(char separator,
                      cz::Allocator vector_allocator,
                      cz::Allocator string_allocator,
                      cz::Vector<cz::Str>* values) const {
    cz::Str remaining = *this;
    while (1) {
        cz::Str value = remaining;
        bool split = remaining.split_excluding(separator, &value, &remaining);

        values->reserve(vector_allocator, 1);
        values->push(value.clone(string_allocator));

        if (!split)
            break;
    }
}

void Str::split_clone(cz::Str separator,
                      cz::Allocator vector_allocator,
                      cz::Allocator string_allocator,
                      cz::Vector<cz::Str>* values) const {
    cz::Str remaining = *this;
    while (1) {
        cz::Str value = remaining;
        bool split = remaining.split_excluding(separator, &value, &remaining);

        values->reserve(vector_allocator, 1);
        values->push(value.clone(string_allocator));

        if (!split)
            break;
    }
}

void Str::split_clone_nt(char separator,
                         cz::Allocator vector_allocator,
                         cz::Allocator string_allocator,
                         cz::Vector<cz::Str>* values) const {
    cz::Str remaining = *this;
    while (1) {
        cz::Str value = remaining;
        bool split = remaining.split_excluding(separator, &value, &remaining);

        values->reserve(vector_allocator, 1);
        values->push(value.clone_null_terminate(string_allocator));

        if (!split)
            break;
    }
}

void Str::split_clone_nt(cz::Str separator,
                         cz::Allocator vector_allocator,
                         cz::Allocator string_allocator,
                         cz::Vector<cz::Str>* values) const {
    cz::Str remaining = *this;
    while (1) {
        cz::Str value = remaining;
        bool split = remaining.split_excluding(separator, &value, &remaining);

        values->reserve(vector_allocator, 1);
        values->push(value.clone_null_terminate(string_allocator));

        if (!split)
            break;
    }
}

void Str::lines(cz::Allocator allocator, cz::Vector<cz::Str>* values) const {
    cz::Str str = *this;
    if (str.ends_with('\n'))
        str = str.slice_end(str.len - 1);
    str.split_into('\n', allocator, values);
}

}
