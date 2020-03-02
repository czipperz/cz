#include <cz/working_directory.hpp>

#include <cz/assert.hpp>
#include <cz/fs/directory.hpp>
#include <cz/path.hpp>
#include <cz/string.hpp>
#include <cz/try.hpp>

#include <ctype.h>
#include <errno.h>

// Although the headers are different, we can use the Windows "Posix"
// implementation to get similar semantics.  We just have to alias the system
// calls to the underscore-prefixed versions.
#ifdef _WIN32
#include <direct.h>
#define getcwd _getcwd
#define chdir _chdir
#else
#include <unistd.h>
#endif

namespace cz {

Result set_working_directory(const char* cstr_path) {
    if (chdir(cstr_path) < 0) {
        return Result::last_error();
    } else {
        return Result::ok();
    }
}

Result get_working_directory(Allocator allocator, String* path) {
    // @RandomConstant: for case where there is no limit to the path size
    size_t size = 128;
    CZ_TRY(path::get_max_len(&size));

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

}