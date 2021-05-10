#include <cz/directory.hpp>

#include <errno.h>
#include <cz/char_type.hpp>
#include <cz/defer.hpp>
#include <cz/try.hpp>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

#include <stdlib.h>

#include <type_traits>
static_assert(std::is_same<void*, HANDLE>::value, "HANDLE must be void* on windows");

namespace cz {

static Result get_last_error() {
    Result result;
    result.code = GetLastError();
    return result;
}

Result Directory_Iterator::advance(Allocator allocator, String* out) {
    WIN32_FIND_DATA data;
    if (FindNextFileA(_handle, &data)) {
        Str file = data.cFileName;
        out->reserve(allocator, file.len + 1);
        out->append(file);
        out->null_terminate();
        return Result::ok();
    } else if (GetLastError() == ERROR_NO_MORE_FILES) {
        _done = true;
        return Result::ok();
    } else {
        return get_last_error();
    }
}

Result Directory_Iterator::drop() {
    if (FindClose(_handle)) {
        return Result::ok();
    } else {
        return get_last_error();
    }
}

Result Directory_Iterator::init(const char* cstr_path, Allocator allocator, String* out) {
    // Windows doesn't list the files in a directory, it findes files matching criteria.
    // Thus we must append `"\*"` to get all files in the directory `cstr_path`.
    Str str_path = cstr_path;
    String path = {};
    path.reserve(allocator, str_path.len + 3);
    path.append(str_path);
    path.append("\\*");
    path.null_terminate();

    WIN32_FIND_DATA data;
    HANDLE handle = FindFirstFileA(path.buffer(), &data);

    path.drop(allocator);

    if (handle == INVALID_HANDLE_VALUE) {
        return get_last_error();
    }

    _handle = handle;

    // skip "." and ".."
    if (strcmp(data.cFileName, ".") == 0 && !FindNextFileA(handle, &data)) {
        if (GetLastError() == ERROR_NO_MORE_FILES) {
            _done = true;
            return Result::ok();
        } else {
            drop();
            return get_last_error();
        }
    }
    if (strcmp(data.cFileName, "..") == 0 && !FindNextFileA(handle, &data)) {
        if (GetLastError() == ERROR_NO_MORE_FILES) {
            _done = true;
            return Result::ok();
        } else {
            drop();
            return get_last_error();
        }
    }

    Str file = data.cFileName;
    out->reserve(allocator, file.len + 1);
    out->append(file);
    out->null_terminate();
    return Result::ok();
}

}

#else
#include <dirent.h>

namespace cz {

Result Directory_Iterator::advance(Allocator allocator, String* out) {
    errno = 0;
    dirent* dirent = readdir((DIR*)_dir);
    if (dirent) {
        Str file = dirent->d_name;
        if (file == "." || file == "..") {
            return advance(allocator);
        }

        out->reserve(file.len + 1);
        out->append(file);
        out->null_terminate();
        return Result::ok();
    } else if (errno == 0) {
        _done = true;
        return Result::ok();
    } else {
        return Result::last_error();
    }
}

Result Directory_Iterator::drop() {
    int ret = closedir((DIR*)_dir);
    (void)ret;
    CZ_DEBUG_ASSERT(ret == 0);
    return Result::ok();
}

Result Directory_Iterator::init(const char* cstr_path, Allocator allocator, String* out) {
    DIR* dir = opendir(cstr_path);
    if (!dir) {
        return Result::last_error();
    }

    _dir = dir;

    Result result = advance(allocator, out);
    if (result.is_err()) {
        closedir(dir);
    }
    return result;
}

}
#endif

namespace cz {

#define define_files_function(STRING, NT)                                                        \
    Result files##NT(Allocator paths_allocator, Allocator path_allocator, const char* cstr_path, \
                     Vector<STRING>* files) {                                                    \
        cz::String file = {};                                                                    \
                                                                                                 \
        Directory_Iterator iterator;                                                             \
        CZ_TRY(iterator.init(cstr_path, path_allocator, &file));                                 \
                                                                                                 \
        while (!iterator.done()) {                                                               \
            file.realloc##NT(path_allocator);                                                    \
                                                                                                 \
            files->reserve(paths_allocator, 1);                                                  \
            files->push(file);                                                                   \
                                                                                                 \
            file = {};                                                                           \
            Result result = iterator.advance(path_allocator, &file);                             \
            if (result.is_err()) {                                                               \
                /* Ignore errors in destruction */                                               \
                iterator.drop();                                                                 \
                return result;                                                                   \
            }                                                                                    \
        }                                                                                        \
                                                                                                 \
        return iterator.drop();                                                                  \
    }

// clang-format off
define_files_function(String,)
define_files_function(Str,)
define_files_function(String, _null_terminate)
define_files_function(Str, _null_terminate)
// clang-format on

}
