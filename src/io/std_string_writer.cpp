#include "std_string_writer.hpp"

namespace cz {
namespace io {

Result sstring_writer_write_str(void* _string, Str str) {
    auto string = static_cast<std::string*>(_string);
    string->append(str.buffer, str.len);
    return Result::ok();
}

Writer sstring_writer(std::string* string) {
    return {{sstring_writer_write_str}, string};
}

}
}
