#include <cz/std_string_writer.hpp>

namespace cz {

static Result std_string_writer_write_str(void* _string, Str str) {
    auto string = static_cast<std::string*>(_string);
    string->append(str.buffer, str.len);
    return Result::ok();
}

Writer std_string_writer(std::string* string) {
    return {std_string_writer_write_str, string};
}

}
