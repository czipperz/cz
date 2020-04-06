#include <cz/fs/directory.hpp>

#include <ctype.h>
#include <cz/defer.hpp>
#include <cz/try.hpp>

#ifdef _WIN32
#include <windows.h>
#undef min
#undef max

#include <stdlib.h>

#include <type_traits>
static_assert(std::is_same<void*, HANDLE>::value, "HANDLE must be void* on windows");

namespace cz {
namespace fs {

static Result get_last_error() {
    Result result;
    result.code = GetLastError();
    return result;
}

Result DirectoryIterator::advance() {
    WIN32_FIND_DATA data;
    if (FindNextFileA(_handle, &data)) {
        _file.set_len(0);

        Str file = data.cFileName;
        _file.append(file);
        _file.null_terminate();

        return Result::ok();
    } else if (GetLastError() == ERROR_NO_MORE_FILES) {
        _done = true;
        return Result::ok();
    } else {
        return get_last_error();
    }
}

Result DirectoryIterator::destroy() {
    _file.drop(_allocator);
    if (FindClose(_handle)) {
        return Result::ok();
    } else {
        return get_last_error();
    }
}

Result DirectoryIterator::create(const char* cstr_path) {
    // Windows doesn't list the files in a directory, it findes files matching criteria.  Thus we
    // must append \c "\*" to get all files in the directory \c cstr_path.
    char buffer[_MAX_PATH];
    Str str_path = cstr_path;
    str_path.len = min(sizeof(buffer) - sizeof("\\*"), str_path.len);
    memcpy(buffer, str_path.buffer, str_path.len);
    strcpy(buffer + str_path.len, "\\*");

    WIN32_FIND_DATA data;
    HANDLE handle = FindFirstFileA(buffer, &data);

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
            destroy();
            return get_last_error();
        }
    }
    if (strcmp(data.cFileName, "..") == 0 && !FindNextFileA(handle, &data)) {
        if (GetLastError() == ERROR_NO_MORE_FILES) {
            _done = true;
            return Result::ok();
        } else {
            destroy();
            return get_last_error();
        }
    }

    CZ_DEBUG_ASSERT(_file.len() == 0);

    // _MAX_PATH (= 260) accounts for null terminator
    _file.reserve(_allocator, sizeof(buffer));

    Str file = data.cFileName;
    _file.append(file);
    _file.null_terminate();

    return Result::ok();
}

}
}

#else
#include <dirent.h>

namespace cz {
namespace fs {

Result DirectoryIterator::advance() {
    errno = 0;
    dirent* dirent = readdir((DIR*)_dir);
    if (dirent) {
        Str file = dirent->d_name;
        if (file == "." || file == "..") {
            return advance();
        }

        _file.set_len(0);
        _file.append(file);
        _file.null_terminate();

        return Result::ok();
    } else if (errno == 0) {
        _done = true;
        return Result::ok();
    } else {
        return Result::last_error();
    }
}

Result DirectoryIterator::destroy() {
    _file.drop(_allocator);
    int ret = closedir((DIR*)_dir);
    (void)ret;
    CZ_DEBUG_ASSERT(ret == 0);
    return Result::ok();
}

Result DirectoryIterator::create(const char* cstr_path) {
    DIR* dir = opendir(cstr_path);
    if (!dir) {
        return Result::last_error();
    }

    // NAME_MAX doesn't account for null terminator
    _file.reserve(_allocator, NAME_MAX + 1);
    _dir = dir;

    auto result = advance();
    if (result.is_err()) {
        closedir(dir);
    }
    return result;
}

}
}
#endif

namespace cz {
namespace fs {

Result files(Allocator paths_allocator,
             Allocator path_allocator,
             const char* cstr_path,
             Vector<String>* paths) {
    DirectoryIterator iterator(paths_allocator);
    CZ_TRY(iterator.create(cstr_path));

    while (!iterator.done()) {
        paths->reserve(paths_allocator, 1);
        paths->push(iterator.file().duplicate(path_allocator));

        auto result = iterator.advance();
        if (result.is_err()) {
            // ignore errors in destruction
            iterator.destroy();
            return result;
        }
    }

    return iterator.destroy();
}

Result files(Allocator paths_allocator,
             Allocator path_allocator,
             const char* cstr_path,
             Vector<Str>* paths) {
    DirectoryIterator iterator(paths_allocator);
    CZ_TRY(iterator.create(cstr_path));

    while (!iterator.done()) {
        paths->reserve(paths_allocator, 1);
        paths->push(iterator.file().duplicate(path_allocator));

        auto result = iterator.advance();
        if (result.is_err()) {
            // ignore errors in destruction
            iterator.destroy();
            return result;
        }
    }

    return iterator.destroy();
}

}
}
