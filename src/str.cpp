#include <cz/str.hpp>

#include <limits.h>
#include <cz/assert.hpp>
#include <cz/char_type.hpp>
#include <cz/string.hpp>

namespace cz {

String Str::clone(Allocator allocator) const {
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

}
