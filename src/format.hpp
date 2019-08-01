#pragma once

#include "error.hpp"
#include "result.hpp"
#include "slice.hpp"
#include "string.hpp"

namespace cz {
namespace format {

using Result = cz::io::Result;

struct Write {
    Result (*write_str)(void* data, Str str);
};

struct Writer {
    Write write;
    void* data;

    Result write_char(char c);
    Result write_str(Str str);
};

Writer string_writer(String* string);

template <class... Ts>
String format(const char* format, Ts... ts) {
    String string;
    auto writer = string_writer(&string);
    write(writer, format, ts...);
    return string;
}

Result write(Writer writer, char);
Result write(Writer writer, Str);

Result write(Writer writer, short);
Result write(Writer writer, unsigned short);
Result write(Writer writer, int);
Result write(Writer writer, unsigned int);
Result write(Writer writer, long);
Result write(Writer writer, unsigned long);
Result write(Writer writer, long long);
Result write(Writer writer, unsigned long long);

struct Address {
    void* val;
};

inline Address addr(void* val) {
    return {val};
}

Result write(Writer writer, Address addr);

template <class T>
struct Debug {
    T val;
};

template <class T>
inline Debug<T> debug(T val) {
    return {val};
}

template <class T>
Result write(Writer writer, Debug<T> debug) {
    return write(writer, debug.val);
}

template <class T>
Result write(Writer writer, Debug<Slice<T>> slice) {
    CZ_TRY(write(writer, '['));

    for (size_t i = 0; i < slice.len; ++i) {
        if (i != 0) {
            CZ_TRY(write(writer, ", "));
        }

        CZ_TRY(write(writer, debug(slice.buffer[i])));
    }

    return write(writer, ']');
}

}
}
