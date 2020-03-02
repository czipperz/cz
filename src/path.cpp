#include <cz/path.hpp>

#include <ctype.h>
#include <cz/try.hpp>
#include <cz/working_directory.hpp>

#ifdef _WIN32
#include <stdlib.h>
#else
#include <errno.h>
#include <unistd.h>
#endif

namespace cz {
namespace path {

Result get_max_len(size_t* size) {
#ifdef _WIN32
    *size = _MAX_PATH;
    return Result::ok();
#else
    errno = 0;

    long path_max = pathconf("/", _PC_PATH_MAX);

    if (path_max == -1) {
        // errno is only set if there is a limit
        if (errno != 0) {
            return Result::last_error();
        } else {
            return Result::ok();
        }
    } else {
        *size = path_max;
        return Result::ok();
    }
#endif
}

Option<Str> directory_component(Str str) {
    const char* ptr = str.rfind('/');
    if (ptr) {
        return {{str.buffer, static_cast<size_t>(ptr + 1 - str.buffer)}};
    } else {
        return {};
    }
}

Option<Str> name_component(Str str) {
    const char* ptr = str.rfind('/');
    if (ptr) {
        if (ptr + 1 < str.end()) {
            return {{ptr + 1, static_cast<size_t>(str.end() - (ptr + 1))}};
        } else {
            return {};
        }
    } else if (str.len > 0) {
        return {str};
    } else {
        return {};
    }
}

void flatten(char* buffer, size_t* len) {
    size_t index = 0;

#ifdef _WIN32
    // Handle \c X:abc by removing the drive here to standardize to the *nix
    // standard (\c /x/y or \c x/y ).
    if (*len >= 2 && isalpha(buffer[0]) && buffer[1] == ':') {
        index += 2;
    }
#endif

    if (*len >= 1 && buffer[index] == '/') {
        index += 1;
    }

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

void flatten(String* string) {
    size_t len = string->len();
    flatten(string->buffer(), &len);
    string->set_len(len);
}

bool is_absolute(Str file) {
#ifdef _WIN32
    return file.len >= 3 && isalpha(file[0]) && file[1] == ':' && file[2] == '/';
#else
    return file.len >= 1 && file[0] == '/';
#endif
}

Result make_absolute(Str file, Allocator allocator, String* path) {
    if (is_absolute(file)) {
        path->reserve(allocator, file.len + 1);
        path->append(file);
        path->null_terminate();
        return Result::ok();
    }

    CZ_TRY(get_working_directory(allocator, path));

#ifdef _WIN32
    // Handle X:relpath
    if (file.len >= 2 && isalpha(file[0]) && file[1] == ':') {
        CZ_DEBUG_ASSERT(path->len() >= 2 && isalpha((*path)[0]) && (*path)[1] == ':');

        // Don't currently support get_working_directory on different drives
        if ((*path)[0] != file[0]) {
            CZ_PANIC(
                "cz::fs::make_absolute(): Unimplemented get_working_directory() for other drives");
        }

        // Remove X: prefix
        file.buffer += 2;
        file.len -= 2;
    }
#endif

    path->reserve(allocator, 1 + file.len + 1);
    path->push('/');
    path->append(file);
    flatten(path);
    path->null_terminate();

    return Result::ok();
}

}
}
