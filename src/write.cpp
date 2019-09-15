#include <cz/write.hpp>

#include <cz/context.hpp>
#include <cz/format.hpp>
#include <cz/try.hpp>

namespace cz {

static Result string_writer_write_str(void* _string, Str str) {
    auto string = static_cast<AllocatedString*>(_string);
    string->reserve(str.len);
    string->append(str);
    return Result::ok();
}

Writer string_writer(AllocatedString* string) {
    return {string_writer_write_str, string};
}

static Result file_writer_write_str(void* _file, Str str) {
    auto file = static_cast<FILE*>(_file);
    auto ret = fwrite(str.buffer, sizeof(char), str.len, file);

    if (ret == str.len) {
        return Result::ok();
    } else {
        return Result::last_error();
    }
}

Writer file_writer(FILE* file) {
    return {file_writer_write_str, file};
}

}
