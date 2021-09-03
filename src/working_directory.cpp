#include <cz/working_directory.hpp>

#include <cz/assert.hpp>
#include <cz/path.hpp>
#include <cz/string.hpp>

#include <cz/char_type.hpp>
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

bool set_working_directory(const char* cstr_path) {
    return chdir(cstr_path) == 0;
}

bool get_working_directory(Allocator allocator, String* path) {
    // @RandomConstant: for case where there is no limit to the path size
    size_t size = 128;
    (void)path::get_max_len(&size);

    path->reserve(allocator, size);

    while (!getcwd(path->end(), (int)size)) {
        if (errno == ERANGE) {
            // get longer length and then retry
            size *= 2;
            path->reserve(allocator, size);
        } else {
            // actual error
            return false;
        }
    }

    path->len = strlen(path->buffer);

#ifdef _WIN32
    cz::path::convert_to_forward_slashes(path);
#endif

    return true;
}

}
