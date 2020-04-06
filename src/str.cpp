#include <cz/str.hpp>

#include <limits.h>
#include <cz/assert.hpp>
#include <cz/string.hpp>

namespace cz {

String Str::duplicate(Allocator allocator) const {
    auto ptr = static_cast<char*>(allocator.alloc({len, 1}).buffer);
    CZ_ASSERT(ptr != nullptr);
    memcpy(ptr, buffer, len);
    return String{ptr, len, len};
}

String Str::duplicate_null_terminate(Allocator allocator) const {
    auto ptr = static_cast<char*>(allocator.alloc({len + 1, 1}).buffer);
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

}
