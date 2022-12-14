#include <cz/path.hpp>

#include <cz/char_type.hpp>
#include <cz/heap_string.hpp>
#include <cz/working_directory.hpp>

#ifdef _WIN32
#include <stdlib.h>
#else
#include <errno.h>
#include <unistd.h>
#endif

namespace cz {
namespace path {

bool get_max_len(size_t* size) {
#ifdef _WIN32
    *size = _MAX_PATH;
    return true;
#else
    errno = 0;

    long path_max = pathconf("/", _PC_PATH_MAX);

    if (path_max == -1) {
        // errno is only set if there is a limit
        return errno == 0;
    } else {
        *size = path_max;
        return true;
    }
#endif
}

bool directory_component(Str path, size_t* directory_end) {
    const char* ptr = path.rfind('/');
    if (!ptr) {
        return false;
    }
    *directory_end = ptr - path.buffer;

    // Keep the trailing `/` with a root path.
    // For example: `/`, `C:/`, and `//server/share/`.
    if (is_root_path(path.slice_end(*directory_end + 1)))
        ++*directory_end;

    return true;
}

bool directory_component(Str path, Str* directory) {
    if (!directory_component(path, &path.len)) {
        return false;
    }
    *directory = path;
    return true;
}

bool pop_component(Str* path) {
    return directory_component(*path, path);
}

bool pop_component(String* path) {
    return directory_component(*path, &path->len);
}

bool pop_name(Str path, size_t* end) {
    size_t len;
    if (!directory_component(path, &len)) {
        return false;
    }

    // Include the trailing slash if directory_component didn't.
    *end = len;
    if (!is_root_path(path.slice_end(len)))
        ++*end;

    return true;
}

bool pop_name(Str path, Str* out) {
    size_t len;
    if (!pop_name(path, &len)) {
        return false;
    }
    *out = {path.buffer, len};
    return true;
}

bool pop_name(Str* path) {
    return pop_name(*path, &path->len);
}

bool pop_name(String* path) {
    return pop_name(*path, &path->len);
}

void push_component(Allocator allocator, String* path, Str name) {
    if (path->ends_with('/')) {
        path->reserve(allocator, name.len);
        path->append(name);
    } else {
        path->reserve(allocator, name.len + 1);
        path->push('/');
        path->append(name);
    }
}
void push_component(Heap_String* path, Str name) {
    push_component(cz::heap_allocator(), path, name);
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
    int directories = 0;

#ifdef _WIN32
    // Handle \c X:abc by removing the drive here to standardize to the *nix
    // standard (\c /x/y or \c x/y ).
    if (*len >= 2 && cz::is_alpha(buffer[0]) && buffer[1] == ':') {
        index += 2;
        protected_start = 2;
    }

    if (is_unc_path({buffer, *len})) {
        // Ignore ../ in protected part of a UNC path.  IE everything
        // before path in: `//server/share/path`.
        directories -= 2;
    }
#endif

    bool is_absolute = false;
    if (index < *len && buffer[index] == '/') {
        is_absolute = true;
        index += 1;

#ifdef _WIN32
        // UNC path
        if (index == 1 && index < *len && buffer[index] == '/')
            ++index;
#endif

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
            } else if (is_absolute && directories == 0) {
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
    flatten(string->buffer, &string->len);
}

/// Test file starts with `//server/share/`.  If not,
/// return 0.  If yes, return the length of this prefix.
static size_t unc_path_prefix_length(Str file) {
    if (file.len < strlen("//a/b/") || !is_dir_sep(file[0]) || !is_dir_sep(file[1]))
        return 0;

    size_t i = 2;

    // Advance over any extra slashes before the `//[server]/share/`.
    for (; i < file.len; ++i) {
        if (!is_dir_sep(file[i]))
            break;
    }

    // Advance over `//[server]/share/`.
    for (; i < file.len; ++i) {
        if (is_dir_sep(file[i]))
            break;
    }

    // Advance over `//server[/]share/`.
    for (; i < file.len; ++i) {
        if (!is_dir_sep(file[i]))
            break;
    }

    // Advance over `//server/[share]/`.
    for (; i < file.len; ++i) {
        if (is_dir_sep(file[i]))
            break;
    }

    // If i at end of string then there is no `//server/share/[/]` so fail.
    if (i == file.len)
        return 0;

    CZ_DEBUG_ASSERT(is_dir_sep(file[i]));
    return i;
}

bool is_unc_path(Str file) {
    size_t result = unc_path_prefix_length(file);
    return result != 0;
}

static bool is_unc_root_path(Str file) {
    size_t unc_i = unc_path_prefix_length(file);
    for (size_t i = unc_i; i < file.len; ++i) {
        if (!is_dir_sep(file[i])) {
            return false;
        }
    }
    return true;
}

bool is_absolute(Str file) {
#ifdef _WIN32
    if (is_unc_path(file)) {
        return true;
    }
    return file.len >= 3 && cz::is_alpha(file[0]) && file[1] == ':' && is_dir_sep(file[2]);
#else
    return file.len >= 1 && file[0] == '/';
#endif
}

bool is_root_path(Str file) {
#ifdef _WIN32
    if (is_unc_root_path(file)) {
        return true;
    }
    return file.len == 3 && cz::is_alpha(file[0]) && file[1] == ':' && is_dir_sep(file[2]);
#else
    return file == "/";
#endif
}

template <class PWD>
static bool generic_make_absolute(Str file, Allocator allocator, String* path, PWD&& pwd) {
    if (is_absolute(file)) {
        path->reserve(allocator, file.len + 1);
    } else {
#ifdef _WIN32
        size_t path_len = path->len;
#endif

        bool success = pwd(allocator, path);
        if (!success)
            return false;

#ifdef _WIN32
        if (file.len >= 2 && cz::is_alpha(file[0]) && file[1] == ':') {
            // Handle X:relpath
            CZ_DEBUG_ASSERT(path->len >= 2 && cz::is_alpha((*path)[0]) && (*path)[1] == ':');
            if (path->len >= 2 && cz::is_alpha((*path)[0]) && (*path)[1] == ':') {
                if (cz::to_lower((*path)[0]) != cz::to_lower(file[0])) {
                    // The working directory is set on a different
                    // drive so pretend this file is absolute.
                    path->len = path_len;
                    path->reserve(allocator, 3 + file.len + 1);
                    path->append(file.slice_end(2));
                }

                // Remove X: prefix
                file.buffer += 2;
                file.len -= 2;
            }
        } else if (file.len >= 1 && is_dir_sep(file[0])) {
            // Handle /drive/rel/path by only taking the X: from the 'path'.
            CZ_DEBUG_ASSERT(path->len >= 2 && cz::is_alpha((*path)[0]) && (*path)[1] == ':');
            if (path->len >= 2 && cz::is_alpha((*path)[0]) && (*path)[1] == ':') {
                path->len = 2;
            }
        }
#endif

        path->reserve(allocator, 1 + file.len + 1);
        path->push('/');
    }

    path->append(file);
    flatten(path);
    path->null_terminate();
    return true;
}

bool make_absolute(Str file, Allocator allocator, String* path) {
    return generic_make_absolute(file, allocator, path, get_working_directory);
}

void make_absolute(Str file, Str working_directory, Allocator allocator, String* path) {
    generic_make_absolute(file, allocator, path, [&](Allocator allocator, String* path) {
        path->reserve(allocator, working_directory.len);
        path->append(working_directory);
        return true;
    });
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
