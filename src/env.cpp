#include <cz/env.hpp>

#include <stdlib.h>

namespace cz {
namespace env {

bool get(const char* key, Allocator allocator, String* value) {
    const char* cstr = getenv(key);
    if (!cstr) {
        return false;
    }

    cz::Str str = cstr;
    value->reserve(allocator, str.len + 1);
    value->append(str);
    value->null_terminate();
    return true;
}

bool set(const char* key, const char* value) {
    return setenv(key, value, /*overwrite=*/true) == 0;
}

bool remove(const char* key) {
    return unsetenv(key) == 0;
}

bool get_home(Allocator allocator, String* value) {
    const char* key = nullptr;
#ifdef _WIN32
    key = "USERPROFILE";
#else
    key = "HOME";
#endif
    return get(key, allocator, value);
}

}
}
