#include <cz/fs/directory.hpp>

#include <ctype.h>
#include <cz/defer.hpp>

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
        _file.clear();

        Str file = data.cFileName;
        _file.append(file);

        CZ_DEBUG_ASSERT(_file.cap() - _file.len() >= 1);
        *_file.end() = '\0';

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

    CZ_DEBUG_ASSERT(_file.cap() - _file.len() >= 1);
    *_file.end() = '\0';

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

        _file.clear();
        _file.append(file);

        CZ_DEBUG_ASSERT(_file.cap() - _file.len() >= 1);
        *_file.end() = '\0';

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

Result files(mem::Allocator allocator, const char* cstr_path, Vector<String>* paths) {
    DirectoryIterator iterator(allocator);
    CZ_TRY(iterator.create(cstr_path));

    while (!iterator.done()) {
        paths->reserve(allocator, 1);
        paths->push(iterator.file().duplicate(allocator));

        auto result = iterator.advance();
        if (result.is_err()) {
            // ignore errors in destruction
            iterator.destroy();
            return result;
        }
    }

    return iterator.destroy();
}

Str directory_component(Str str) {
    const char* ptr = str.rfind('/');
    if (ptr) {
        return {str.buffer, static_cast<size_t>(ptr + 1 - str.buffer)};
    } else {
        return {};
    }
}

void flatten_path(char* buffer, size_t* len) {
    size_t index = 0;

#ifdef _WIN32
    if (*len >= 3 && isalpha(buffer[0]) && buffer[1] == ':' && buffer[2] == '/') {
        index += 3;
    } else if (*len >= 1 && buffer[0] == '/') {
        index += 1;
    }
#else
    if (*len >= 1 && buffer[0] == '/') {
        index += 1;
    }
#endif

    size_t directories = 0;

    while (1) {
        if (index + 1 < *len && buffer[index] == '.' && buffer[index + 1] == '.' &&
            (index + 2 == *len || buffer[index + 2] == '/')) {
            if (directories > 0) {
                const char* point = Str{buffer, index - 1}.rfind('/');

                size_t start;
                if (point) {
                    start = point - buffer + 1;
                } else {
                    start = 0;
                }

                // abc/def/..
                //     ------
                // abc/def/../ghi
                //     -------
                size_t end = index + 2 + (index + 2 != *len);
                memmove(buffer + start, buffer + end, *len - end);
                *len -= end - start;
                index = start;
                --directories;
            } else {
                index += 2 + (index + 2 != *len);
            }
        } else if (index < *len && buffer[index] == '.' &&
                   (index + 1 == *len || buffer[index + 1] == '/')) {
            size_t offset = 1 + (index + 1 != *len);
            memmove(buffer + index, buffer + index + offset, *len - index - offset);
            *len -= offset;
        } else {
            const char* point = Str{buffer + index, *len - index}.find('/');
            if (!point) {
                break;
            }

            // get the point after the /
            index = point - buffer + 1;

            ++directories;
        }
    }
}
}
}
