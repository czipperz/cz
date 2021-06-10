#include <cz/path.hpp>

#include <cz/char_type.hpp>
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

bool directory_component(Str path, size_t* directory_end) {
    const char* ptr = path.rfind('/');
    if (!ptr) {
        return false;
    }
    *directory_end = ptr - path.buffer;
    return true;
}

bool directory_component(Str path, Str* directory) {
    if (!directory_component(path.buffer, &path.len)) {
        return false;
    }
    *directory = path;
    return true;
}

bool pop_component(Str* path) {
    return directory_component(*path, path);
}

bool pop_component(String* path) {
    size_t len = path->len();
    if (!directory_component(path->buffer(), &len)) {
        return false;
    }
    path->set_len(len);
    return true;
}

bool name_component(Str path, size_t* name_start) {
    const char* ptr = path.rfind('/');

    // No forward slashes -> all the name component.
    if (!ptr) {
        *name_start = 0;
        return true;
    }

    // Path ends in foward slash -> no name component.
    if (ptr + 1 == path.end()) {
        return false;
    }

    // Has name component and directory component.
    *name_start = ptr - path.buffer + 1;
    return true;
}

bool name_component(Str path, Str* name) {
    size_t name_start;
    if (!name_component(path, &name_start)) {
        return false;
    }
    *name = path.slice_start(name_start);
    return true;
}

bool directory_component(const char* path, size_t* len) {
    const char* slash = cz::Str{path, *len}.rfind('/');
    if (!slash) {
        return false;
    }

    *len = slash - path;
    return true;
}

bool has_component(Str path, Str component) {
    while (1) {
        const char* ptr = path.find(component);
        if (!ptr) {
            return false;
        }

        if ((ptr == path.start() || ptr[-1] == '/') &&
            (ptr + component.len == path.end() || ptr[component.len] == '/')) {
            return true;
        }

        path = path.slice_start(ptr + 1);
    }
}

void flatten(char* buffer, size_t* len) {
    size_t index = 0;
    size_t protected_start = 0;

#ifdef _WIN32
    // Handle \c X:abc by removing the drive here to standardize to the *nix
    // standard (\c /x/y or \c x/y ).
    if (*len >= 2 && cz::is_alpha(buffer[0]) && buffer[1] == ':') {
        index += 2;
        protected_start = 2;
    }
#endif

    bool is_absolute = false;
    if (index < *len && buffer[index] == '/') {
        is_absolute = true;
        index += 1;

        // Remove multiple / in a row.
        size_t end = index;
        while (end < *len && buffer[end] == '/') {
            ++end;
        }
        if (end > index) {
            memmove(buffer + index, buffer + end, *len - end);
            *len -= end - index;
        }
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
                    start = protected_start;
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
            } else if (is_absolute) {
                // Delete ../ at start if we are absolute.
                size_t end = index + 2 + (index + 2 != *len);
                memmove(buffer + index, buffer + end, *len - end);
                *len -= end - index;
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

            // Remove multiple / in a row.
            size_t end = index;
            while (end < *len && buffer[end] == '/') {
                ++end;
            }
            if (end > index) {
                memmove(buffer + index, buffer + end, *len - end);
                *len -= end - index;
            }

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
    return file.len >= 3 && cz::is_alpha(file[0]) && file[1] == ':' && file[2] == '/';
#else
    return file.len >= 1 && file[0] == '/';
#endif
}

Result make_absolute(Str file, Allocator allocator, String* path) {
    if (is_absolute(file)) {
        path->reserve(allocator, file.len + 1);
    } else {
        CZ_TRY(get_working_directory(allocator, path));

#ifdef _WIN32
        // Handle X:relpath
        if (file.len >= 2 && cz::is_alpha(file[0]) && file[1] == ':') {
            CZ_DEBUG_ASSERT(path->len() >= 2 && cz::is_alpha((*path)[0]) && (*path)[1] == ':');

            // Don't currently support get_working_directory on different drives
            if (cz::to_lower((*path)[0]) != cz::to_lower(file[0])) {
                CZ_PANIC(
                    "cz::fs::make_absolute(): Unimplemented get_working_directory() for other "
                    "drives");
            }

            // Remove X: prefix
            file.buffer += 2;
            file.len -= 2;
        }
#endif

        path->reserve(allocator, 1 + file.len + 1);
        path->push('/');
    }

    path->append(file);
    flatten(path);
    path->null_terminate();
    return Result::ok();
}

void convert_to_forward_slashes(char* path, size_t len) {
    size_t index = 0;
    while (1) {
        char* bs = (char*)memchr(path + index, '\\', len - index);
        if (!bs) {
            return;
        }
        *bs = '/';
        index = bs - path + 1;
    }
}

void convert_to_back_slashes(char* path, size_t len) {
    size_t index = 0;
    while (1) {
        char* bs = (char*)memchr(path + index, '/', len - index);
        if (!bs) {
            return;
        }
        *bs = '\\';
        index = bs - path + 1;
    }
}

}
}
