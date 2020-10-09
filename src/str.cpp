#include <cz/str.hpp>

#include <limits.h>
#include <cz/assert.hpp>
#include <cz/string.hpp>

namespace cz {

String Str::duplicate(Allocator allocator) const {
    auto ptr = static_cast<char*>(allocator.alloc({len, 1}));
    CZ_ASSERT(ptr != nullptr);
    memcpy(ptr, buffer, len);
    return String{ptr, len, len};
}

String Str::duplicate_null_terminate(Allocator allocator) const {
    auto ptr = static_cast<char*>(allocator.alloc({len + 1, 1}));
    CZ_ASSERT(ptr != nullptr);
    memcpy(ptr, buffer, len);
    ptr[len] = '\0';
    return String{ptr, len, len + 1};
}

const char* Str::rfind(char pattern) const {
#ifdef _GNU_SOURCE
    return static_cast<const char*>(::memrchr(buffer, pattern, len));
#else
    for (size_t i = len; i > 0; --i) {
        if (buffer[i - 1] == pattern) {
            return &buffer[i - 1];
        }
    }
    return nullptr;
#endif
}

const char* Str::find(cz::Str infix) const {
#ifdef _GNU_SOURCE
    return static_cast<const char*>(::memmem(buffer, len, infix.buffer, infix.len));
#else
    // todo: optimize this
    for (size_t i = 0; i + infix.len <= len; ++i) {
        if (memcmp(buffer + i, infix.buffer, infix.len) == 0) {
            return buffer + i;
        }
    }
    return nullptr;
#endif
}

}
