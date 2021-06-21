#include <cz/find_file.hpp>

#include <cz/file.hpp>
#include <cz/path.hpp>

namespace cz {

bool find_file_up(Allocator allocator, String* path, Str file) {
    if (path->ends_with('/')) {
        path->pop();
    }

    path->reserve(allocator, file.len + 2);
    path->push('/');

    while (1) {
        size_t old_len = path->len();
        path->append(file);
        path->null_terminate();

        if (file::does_file_exist(path->buffer())) {
            return true;
        }

        path->set_len(old_len - 1);
        if (!path::pop_name(path)) {
            return false;
        }
    }
}

bool find_dir_with_file_up(Allocator allocator, String* path, Str file) {
    if (!find_file_up(allocator, path, file)) {
        return false;
    }

    path->set_len(path->len() - file.len - 1);
    path->null_terminate();
    return true;
}

}
