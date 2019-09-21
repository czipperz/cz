#include <cz/fs/working_directory.hpp>

#include <cz/assert.hpp>
#include <cz/fs/directory.hpp>
#include <cz/string.hpp>
#include <cz/try.hpp>

#include <errno.h>

// Although the headers are different, we can use the Windows "Posix" implementation to get similar
// semantics.  We have to alias the system calls to the underscore-prefixed versions.  And we have
// to use _MAX_PATH macro instead of using pathconf.
#ifdef _WIN32
#include <direct.h>
#define getcwd _getcwd
#define chdir _chdir
#else
#include <unistd.h>
#endif

namespace cz {
namespace fs {

Result set_working_directory(const char* cstr_path) {
    if (chdir(cstr_path) < 0) {
        return Result::last_error();
    } else {
        return Result::ok();
    }
}

static Result get_path_max(size_t* size) {
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
            // @RandomConstant: no limit to the path size
            path_max = 128;
        }
    }

    *size = path_max;

    return Result::ok();
#endif
}

Result get_working_directory(Allocator allocator, String* path) {
    size_t size;
    CZ_TRY(get_path_max(&size));

    path->set_len(0);
    path->reserve(allocator, size);

    while (!getcwd(path->end(), size)) {
        if (errno == ERANGE) {
            // get longer length and then retry
            size *= 2;
            path->reserve(allocator, size);
        } else {
            // actual error
            return Result::last_error();
        }
    }

    path->set_len(strlen(path->buffer()));

#ifdef _WIN32
    // replace '\' with '/' on windows
    for (size_t i = 0; i < path->len(); ++i) {
        if ((*path)[i] == '\\') {
            (*path)[i] = '/';
        }
    }
#endif

    return Result::ok();
}

static bool is_absolute(Str file) {
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
    flatten_path(path);
    path->null_terminate();

    return Result::ok();
}

}
}
