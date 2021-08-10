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

bool get_path(Allocator vector_allocator, Allocator value_allocator, cz::Vector<Str>* values) {
    cz::String string = {};
    if (!get("PATH", value_allocator, &string)) {
        return false;
    }

    char split = 0;
#ifdef _WIN32
    split = ';';
#else
    split = ':';
#endif

    string.split_into(split, vector_allocator, values);
    return true;
}

bool get_path_extensions(Allocator vector_allocator,
                         Allocator value_allocator,
                         cz::Vector<Str>* values) {
    cz::String string = {};
#ifdef _WIN32
    if (!get("PATHEXT", value_allocator, &string)) {
        return false;
    }
#else
    string.reserve(value_allocator, 1);
    string.null_terminate();
#endif

    string.split_into(';', vector_allocator, values);
    return true;
}

}
}
