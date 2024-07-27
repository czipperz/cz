#include <cz/path.hpp>

#include <stdlib.h>
#include <cz/char_type.hpp>
#include <cz/defer.hpp>
#include <cz/env.hpp>
#include <cz/heap_string.hpp>
#include <cz/working_directory.hpp>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#else
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <sys/stat.h>
#include <sys/types.h>
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

cz::String standardize_path(cz::Allocator allocator, cz::Str user_path) {
    cz::String user_path_nt = {};
    CZ_DEFER(user_path_nt.drop(cz::heap_allocator()));

    // Dereference home directory.
    if (user_path.starts_with("~") && env::get_home(cz::heap_allocator(), &user_path_nt)) {
        user_path_nt.reserve(cz::heap_allocator(), user_path.len - 1 + 1);
        user_path_nt.append(user_path.slice_start(1));
        user_path_nt.null_terminate();
    } else {
        user_path_nt.reserve(cz::heap_allocator(), user_path.len + 1);
        user_path_nt.append(user_path);
        user_path_nt.null_terminate();
    }

#ifndef _WIN32
    // Don't dereference any symbolic links in `/proc` or `/dev` because the
    // symbolic links are often broken.  Example usage is `mag <(git diff)` will
    // open `/proc/self/fd/%d` or `/dev/fd/%d` with the result of the subcommand (`git diff`).
    if (user_path_nt.starts_with("/proc/") || user_path_nt.starts_with("/dev/")) {
        cz::path::convert_to_forward_slashes(&user_path_nt);

        cz::String path = {};
        cz::path::make_absolute(user_path_nt, allocator, &path);
        if (path[path.len - 1] == '/') {
            path.pop();
        }
        path.null_terminate();
        return path;
    }
#endif

    // Use the kernel functions to standardize the path if they work.
#ifdef _WIN32
    {
        // Open the file in read mode.
        HANDLE handle = CreateFile(user_path_nt.buffer, GENERIC_READ, FILE_SHARE_READ, nullptr,
                                   OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (handle != INVALID_HANDLE_VALUE) {
            CZ_DEFER(CloseHandle(handle));

            cz::String buffer = {};
            buffer.reserve(allocator, MAX_PATH);
            while (1) {
                // Get the standardized file name.
                DWORD res = GetFinalPathNameByHandleA(handle, buffer.buffer, (DWORD)buffer.cap, 0);

                if (res <= 0) {
                    // Failure so stop.
                    break;
                } else if (res < buffer.cap) {
                    // Success.
                    buffer.len = res;

                    // Remove the "\\?\UNC\" or "\\?\" prefix.
                    if (buffer.starts_with("\\\\?\\UNC\\"))
                        buffer.remove_many(2, 6);  // Remove the '?\UNC\'.
                    else
                        buffer.remove_many(0, 4);

                    cz::path::convert_to_forward_slashes(&buffer);

                    buffer.null_terminate();
                    return buffer;
                } else {
                    // Retry with capacity as res.
                    buffer.reserve(allocator, res);
                }
            }
            buffer.drop(allocator);
        }
    }
#elif _BSD_SOURCE || _XOPEN_SOURCE >= 500 || _XOPEN_SOURCE && _XOPEN_SOURCE_EXTENDED
    // ^ Feature test for realpath.
    {
        char* ptr = realpath(user_path_nt.buffer, nullptr);
        if (ptr) {
            CZ_DEFER(free(ptr));
            return cz::Str{ptr}.clone_null_terminate(allocator);
        }
    }
#endif

    // Fallback to doing it ourselves.

#ifdef _WIN32
    cz::path::convert_to_forward_slashes(&user_path_nt);
#endif

    cz::String path = {};
    CZ_DEFER(path.drop(cz::heap_allocator()));
    cz::path::make_absolute(user_path_nt, cz::heap_allocator(), &path);
    if (path[path.len - 1] == '/') {
        path.pop();
    }
    path.null_terminate();

    cz::String result = {};
    result.reserve(allocator, path.len + 1);

#ifdef _WIN32
    // TODO: support symbolic links on Windows.

    size_t start = 0;

    if (cz::is_alpha(path[0])) {
        // Path looks like X:/path
        start = 3;

        // Append drive as uppercase.
        CZ_DEBUG_ASSERT(cz::is_alpha(path[0]));
        CZ_DEBUG_ASSERT(path[1] == ':');
        CZ_DEBUG_ASSERT(path.len == 2 || path[2] == '/');
        result.push(cz::to_upper(path[0]));
        result.push(':');

        // Only append the forward slash now if there are no components.
        if (path.len <= 3) {
            result.push('/');
        }
    } else {
        // UNC path; looks like //server/share/path (we only care about //server/ here).
        for (; start < path.len; ++start) {
            if (path[start] != '/')
                break;
        }
        size_t start_server = start;
        for (; start < path.len; ++start) {
            if (path[start] == '/')
                break;
        }
        size_t end_server = start;
        for (; start < path.len; ++start) {
            if (path[start] != '/')
                break;
        }

        cz::Str server = path.slice(start_server, end_server);
        result.append("//");
        for (size_t i = 0; i < server.len; ++i) {
            result.push(cz::to_lower(server[i]));
        }

        // Only append the forward slash now if there are no components.
        if (start == path.len)
            result.push('/');
    }

    // Step through each component of the path and fix the capitalization.
    while (1) {
        // Advance over forward slashes.
        while (start < path.len && path[start] == '/') {
            ++start;
        }
        if (start >= path.len) {
            break;
        }

        // Find end of component.
        size_t end = start;
        while (end < path.len && path[end] != '/') {
            ++end;
        }

        // Temporarily terminate the string at the end point.
        char swap = '\0';
        if (end < path.len) {
            cz::swap(swap, path[end]);
        }

        // Find the file on disk.
        WIN32_FIND_DATAA find_data;
        HANDLE handle = FindFirstFile(path.buffer, &find_data);

        if (end < path.len) {
            cz::swap(swap, path[end]);
        }

        // If the find failed then just use the rest of the path as is.
        if (handle == INVALID_HANDLE_VALUE) {
            cz::Str rest_of_path = path.slice_start(start);
            result.reserve(allocator, rest_of_path.len + 1);
            result.push('/');
            result.append(rest_of_path);
            break;
        }

        FindClose(handle);

        // The find succeeded so get the proper component spelling and append it.
        cz::Str proper_component = find_data.cFileName;
        result.reserve(allocator, proper_component.len + 1);
        result.push('/');
        result.append(proper_component);

        start = end;
    }
#else
    // Path stores the components we have already dereferenced.
    result.reserve(allocator, path.len);

    // path stores the path we are trying to test.
    // temp_path will store the result of one readlink call.
    cz::String temp_path = {};
    temp_path.reserve(cz::heap_allocator(), path.len);
    CZ_DEFER(temp_path.drop(cz::heap_allocator()));

    // Try to dereference each component of the path as a symbolic link.  If any
    // component is a symbolic link it and the ones before it are replaced by
    // the link's contents.  Thus we iterate the path in reverse.

    while (1) {
        // Try to read the link.
        ssize_t res;
        const size_t max_dereferences = 5;
        size_t dereference_count = 0;
        while (1) {
            // Dereference the symbolic link.
            res = readlink(path.buffer, temp_path.buffer, temp_path.cap);

            // If we had an error, stop.
            if (res < 0) {
                break;
            }

            // Retry with a bigger buffer.
            if ((size_t)res == temp_path.cap) {
                temp_path.reserve_total(cz::heap_allocator(), temp_path.cap * 2);
                continue;
            }

            // Store the result in path.
            temp_path.len = res;

            if (cz::path::is_absolute(temp_path)) {
                // Discard the directory of the symlink and since it is an absolute path.
                cz::swap(temp_path, path);

                // Pop off trailing forward slashes.
                while (path.ends_with('/')) {
                    path.pop();
                }
            } else {
                // Expand the symlink from the directory it is in.
                path.reserve(cz::heap_allocator(), temp_path.len + 5);
                path.append("/../");
                path.append(temp_path);
            }

            // The symlink may use relative paths so flatten it out now.
            cz::path::flatten(&path);
            path.null_terminate();

            // Prevent infinite loops by stopping after a set count.
            if (dereference_count == max_dereferences) {
                break;
            }

            // Try dereferencing again.
            ++dereference_count;
        }

        size_t offset = 0;
        // Advance through the text part of the component.
        while (offset < path.len && path[path.len - offset - 1] != '/') {
            ++offset;
        }
        // Advance through forward slashes.
        while (offset < path.len && path[path.len - offset - 1] == '/') {
            ++offset;
        }

        // Push the component onto the path.
        result.reserve(allocator, offset);
        result.insert(0, path.slice_start(path.len - offset));

        if (offset >= path.len) {
            break;
        }

        // And chop the component off the path.
        path.len = path.len - offset;
        path.null_terminate();
    }
#endif

    result.reserve(allocator, 1);
    result.null_terminate();
    return result;
}

}
}
