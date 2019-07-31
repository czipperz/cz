#include "format.hpp"

namespace cz {
namespace format {

static Result string_writer_write_str(void* _string, Str str) {
    auto string = static_cast<String*>(_string);
    string->append(str);
    return Result::Ok;
}

Writer string_writer(String* string) {
    return {{string_writer_write_str}, string};
}

Result Writer::write_char(char c) {
    return write_str({&c, 1});
}

Result Writer::write_str(Str str) {
    return write.write_str(data, str);
}

Result write(Writer writer, Str str) {
    return writer.write_str(str);
}

}
}
