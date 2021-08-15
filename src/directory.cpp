#include <cz/directory.hpp>

#include <errno.h>
#include <cz/char_type.hpp>
#include <cz/defer.hpp>
#include <cz/heap.hpp>

#ifdef TRACY_ENABLE
#include <Tracy.hpp>
#else
#define ZoneScoped
#endif

#include <stdlib.h>
#include <string.h>

namespace cz {

int Directory_Iterator::init(const char* cstr_path) {
    ZoneScoped;

#ifdef _WIN32
    HANDLE handle;

    {
        // Windows doesn't list the files in a directory, it findes files matching criteria.
        // Thus we must append `"\*"` to get all files in the directory `cstr_path`.
        String path = {};
        CZ_DEFER(path.drop(heap_allocator()));
        Str str_path = cstr_path;
        path.reserve_exact(heap_allocator(), str_path.len + 3);
        path.append(str_path);
        path.append("\\*");
        path.null_terminate();

        handle = FindFirstFileA(path.buffer, &entry);
    }

    if (handle == INVALID_HANDLE_VALUE) {
        return -1;
    }

    directory = handle;

    // . and .. are always first if they are present.
    if (!strcmp(entry.cFileName, ".")) {
        if (!FindNextFileA(handle, &entry)) {
            if (GetLastError() == ERROR_NO_MORE_FILES) {
                return 0;
            } else {
                drop();
                return -1;
            }
        }
    }
    if (!strcmp(entry.cFileName, "..")) {
        if (!FindNextFileA(handle, &entry)) {
            if (GetLastError() == ERROR_NO_MORE_FILES) {
                // Note: don't drop so we'll get the same error on `next`.
                return 0;
            } else {
                drop();
                return -1;
            }
        }
    }

    return 1;
#else
    DIR* dir = opendir(cstr_path);
    if (!dir) {
        return -1;
    }
    directory = dir;

    int result = advance();

    if (result < 0) {
        drop();
    }

    return result;
#endif
}

bool Directory_Iterator::drop() {
    ZoneScoped;

#ifdef _WIN32
    bool success = FindClose(directory);
    return success;
#else
    int ret = closedir(directory);
    return ret == 0;
#endif
}

int Directory_Iterator::advance() {
    ZoneScoped;

#ifdef _WIN32
    if (FindNextFileA(directory, &entry)) {
        return 1;
    } else if (GetLastError() == ERROR_NO_MORE_FILES) {
        return 0;
    } else {
        return -1;
    }
#else
    errno = 0;
    dirent* dirent = readdir(directory);
    if (dirent) {
        // Skip . and ..
        if (!strcmp(dirent->d_name, ".") || !strcmp(dirent->d_name, "..")) {
            return advance();
        }

        entry = dirent;
        return 1;
    } else if (errno == 0) {
        return 0;
    } else {
        return -1;
    }
#endif
}

const char* Directory_Iterator::get_name() const {
#ifdef _WIN32
    return entry.cFileName;
#else
    return entry->d_name;
#endif
}

Str Directory_Iterator::str_name() const {
    return get_name();
}

void Directory_Iterator::append_name(Allocator allocator, String* string) const {
    Str name = str_name();
    string->reserve_exact(allocator, name.len + 1);
    string->append(name);
    string->null_terminate();
}

#define define_files_function(STRING, FIELD)                                          \
    bool files(Allocator paths_allocator, Allocator path_allocator, const char* path, \
               Vector<STRING>* files) {                                               \
        Directory_Iterator iterator;                                                  \
        int result = iterator.init(path);                                             \
        if (result <= 0)                                                              \
            return result == 0;                                                       \
                                                                                      \
        while (1) {                                                                   \
            cz::String file = iterator.str_name().clone_null_terminate(path_allocator);              \
            files->reserve(paths_allocator, 1);                                       \
            files->push(file FIELD);                                                  \
                                                                                      \
            result = iterator.advance();                                              \
            if (result <= 0) {                                                        \
                if (result == 0) {                                                    \
                    /* No more entries. */                                            \
                    return iterator.drop();                                           \
                } else {                                                              \
                    /* Iteration failed in middle. */                                 \
                    iterator.drop();                                                  \
                    return false;                                                     \
                }                                                                     \
            }                                                                         \
        }                                                                             \
    }

// clang-format off
define_files_function(String,)
define_files_function(Str,)
define_files_function(const char*, .buffer)
// clang-format on

}
