#include <cz/env.hpp>

#include <stdlib.h>
#include <algorithm>
#include <cz/defer.hpp>
#include <cz/file.hpp>
#include <cz/heap.hpp>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#endif

namespace cz {
namespace env {

bool get(const char* key, Allocator allocator, String* value) {
#ifdef _WIN32
    DWORD size = GetEnvironmentVariableA(key, nullptr, 0);
    if (size <= 0) {
        return false;
    }

    value->reserve(allocator, size);
    GetEnvironmentVariableA(key, value->end(), (DWORD)value->remaining());
    value->len += size - 1;
    return true;
#else
    const char* cstr = getenv(key);
    if (!cstr) {
        return false;
    }

    Str str = cstr;
    value->reserve(allocator, str.len + 1);
    value->append(str);
    value->null_terminate();
    return true;
#endif
}

bool set(const char* key, const char* value) {
#ifdef _WIN32
    return SetEnvironmentVariableA(key, value) != 0;
#else
    return setenv(key, value, /*overwrite=*/true) == 0;
#endif
}

bool remove(const char* key) {
#ifdef _WIN32
    return SetEnvironmentVariableA(key, nullptr) != 0;
#else
    return unsetenv(key) == 0;
#endif
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

bool get_path(Allocator vector_allocator,
              Allocator value_allocator,
              Vector<Str>* values,
              String* total) {
    if (!get("PATH", value_allocator, total)) {
        return false;
    }

    char split = 0;
#ifdef _WIN32
    split = ';';
#else
    split = ':';
#endif

    total->split_into(split, vector_allocator, values);
    return true;
}

bool get_path_extensions(Allocator vector_allocator,
                         Allocator value_allocator,
                         Vector<Str>* values,
                         String* total) {
#ifdef _WIN32
    if (!get("PATHEXT", value_allocator, total)) {
        return false;
    }
#else
    total->reserve(value_allocator, 1);
    total->null_terminate();
#endif

    total->split_into(';', vector_allocator, values);
    return true;
}

bool in_path(Str file) {
    String path_total = {};
    String pathext_total = {};
    CZ_DEFER(path_total.drop(cz::heap_allocator()));
    CZ_DEFER(pathext_total.drop(cz::heap_allocator()));
    Vector<Str> paths = {};
    Vector<Str> pathexts = {};
    CZ_DEFER(paths.drop(cz::heap_allocator()));
    CZ_DEFER(pathexts.drop(cz::heap_allocator()));

    if (!get_path(heap_allocator(), heap_allocator(), &paths, &path_total)) {
        return false;
    }

    if (!get_path_extensions(heap_allocator(), heap_allocator(), &pathexts, &pathext_total)) {
        return false;
    }

    size_t max_path = 0;
    for (size_t i = 0; i < paths.len; ++i) {
        max_path = std::max(max_path, paths[i].len);
    }

    size_t max_pathext = 0;
    for (size_t i = 0; i < pathexts.len; ++i) {
        max_pathext = std::max(max_pathext, pathexts[i].len);
    }

    String test = {};
    test.reserve(cz::heap_allocator(), max_path + max_pathext + 1 + file.len + 1);
    CZ_DEFER(test.drop(cz::heap_allocator()));
    for (size_t p = 0; p < paths.len; ++p) {
        test.len = 0;
        test.append(paths[p]);
        test.push('/');
        test.append(file);
        size_t baselen = test.len;

        for (size_t pe = 0; pe < pathexts.len; ++pe) {
            test.len = baselen;
            test.append(pathexts[pe]);
            test.null_terminate();

            if (cz::file::exists(test.buffer)) {
                return true;
            }
        }
    }

    return false;
}

}
}
