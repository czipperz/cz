#include "write.hpp"

#include <stdint.h>
#include "../error.hpp"

namespace cz {
namespace io {

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

Result write(Writer writer, char c) {
    return writer.write_char(c);
}

Result write(Writer writer, Str str) {
    return writer.write_str(str);
}

#define define_write_signed_numeric(type)       \
    Result write(Writer writer, type v) {       \
        if (v < 0) {                            \
            CZ_TRY(write(writer, '-'));         \
        }                                       \
        return write(writer, (unsigned type)v); \
    }

#define define_write_unsigned_numeric(type)        \
    Result write(Writer writer, unsigned type v) { \
        if (v == 0) {                              \
            return write(writer, '0');             \
        }                                          \
        char buffer[32];                           \
        size_t index;                              \
        for (index = 0; v != 0; ++index) {         \
            buffer[index] = '0' + v % 10;          \
            v /= 10;                               \
        }                                          \
        while (index > 0) {                        \
            --index;                               \
            CZ_TRY(write(writer, buffer[index]));  \
        }                                          \
        return Result::Ok;                         \
    }

// clang-format off
define_write_signed_numeric(short)
define_write_signed_numeric(int)
define_write_signed_numeric(long)
define_write_signed_numeric(long long)

define_write_unsigned_numeric(short)
define_write_unsigned_numeric(int)
define_write_unsigned_numeric(long)
define_write_unsigned_numeric(long long)

Result write(Writer writer, Address addr) {
    // clang-format on
    if (addr.val == NULL) {
        return write(writer, "NULL");
    }

    CZ_TRY(write(writer, '0'));
    CZ_TRY(write(writer, 'x'));

    auto v = (intptr_t)addr.val;
    char buffer[32];
    size_t index;
    for (index = 0; v != 0; ++index) {
        buffer[index] = '0' + v % 16;
        v /= 16;
    }
    while (index > 0) {
        --index;
        CZ_TRY(write(writer, buffer[index]));
    }
    return Result::Ok;
}

}
}
