#include <stdint.h>
#include <cz/format.hpp>
#include <cz/write.hpp>

namespace cz {

#define BUFFER_SIZE 32

template <class T>
static size_t write_buffer(char* buffer, T val, T base) {
    const char lookup[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    size_t index;
    for (index = BUFFER_SIZE - 1; val != 0; --index) {
        buffer[index] = lookup[val % base];
        val /= base;
    }
    ++index;
    return index;
}

template <class T>
static Result write_base(Writer writer, T val, T base) {
    if (val == 0) {
        return write(writer, '0');
    }

    char buffer[BUFFER_SIZE];
    size_t index = write_buffer(buffer, val, base);
    return write(writer, cz::Str{buffer + index, sizeof(buffer) - index});
}

#define define_numeric_write(type)                                                       \
    Result write(Writer writer, type val) {                                              \
        if (val < 0) {                                                                   \
            CZ_TRY(write(writer, '-'));                                                  \
            val = -val;                                                                  \
        }                                                                                \
                                                                                         \
        return write(writer, static_cast<unsigned type>(val));                           \
    }                                                                                    \
                                                                                         \
    Result write(Writer writer, unsigned type val) {                                     \
        return write_base<unsigned type>(writer, val, 10);                               \
    }                                                                                    \
                                                                                         \
    Result write(Writer writer, format::WidthSpecified<type> v) {                        \
        if (v.val < 0) {                                                                 \
            CZ_TRY(write(writer, '-'));                                                  \
            v.val = -v.val;                                                              \
        }                                                                                \
                                                                                         \
        return write(writer, format::width(v.width, static_cast<unsigned type>(v.val))); \
    }                                                                                    \
                                                                                         \
    Result write(Writer writer, format::WidthSpecified<unsigned type> v) {               \
        if (v.val == 0) {                                                                \
            for (size_t i = 0; i < v.width; ++i) {                                       \
                CZ_TRY(write(writer, '0'));                                              \
            }                                                                            \
            return Result::ok();                                                         \
        }                                                                                \
                                                                                         \
        char buffer[BUFFER_SIZE];                                                        \
        size_t index = write_buffer<unsigned type>(buffer, v.val, 10);                   \
        if (sizeof(buffer) - v.width < index) {                                          \
            for (size_t i = sizeof(buffer) - v.width; i < index; ++i) {                  \
                buffer[i] = '0';                                                         \
            }                                                                            \
            index = sizeof(buffer) - v.width;                                            \
        }                                                                                \
        return write(writer, cz::Str{buffer + index, sizeof(buffer) - index});           \
    }

// clang-format off
define_numeric_write(short)
define_numeric_write(int)
define_numeric_write(long)
define_numeric_write(long long)

Result write(Writer writer, format::Address addr) {
    // clang-format on
    if (addr.val == nullptr) {
        return write(writer, "nullptr");
    }

    CZ_TRY(write(writer, '0'));
    CZ_TRY(write(writer, 'x'));

    return write_base<intptr_t>(writer, (intptr_t)addr.val, 16);
}

}
