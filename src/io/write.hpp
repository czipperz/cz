#pragma once

#include <stdio.h>
#include "../slice.hpp"
#include "../string.hpp"
#include "../try.hpp"
#include "result.hpp"

namespace cz {
namespace io {

struct Write {
    Result (*write_str)(C* c, void* data, Str str);
};

struct Writer {
    Write write;
    void* data;

    Result write_str(C* c, Str str) { return write.write_str(c, data, str); }
};

Writer file_writer(FILE* file);
Writer string_writer(String* string);
inline Writer cout() { return file_writer(stdout); }
inline Writer cerr() { return file_writer(stderr); }

template <class T1, class T2, class... Ts>
Result write(C* c, Writer writer, T1 t1, T2 t2, Ts... ts) {
    CZ_TRY(write(c, writer, t1));
    return write(c, writer, t2, ts...);
}

template <class... Ts>
Result put(C* c, Ts... ts) {
    return write(c, cout(), ts..., '\n');
}

inline Result write(C* c, Writer writer, char ch) {
    return writer.write_str(c, {&ch, 1});
}
inline Result write(C* c, Writer writer, Str str) {
    return writer.write_str(c, str);
}

Result write(C* c, Writer writer, short);
Result write(C* c, Writer writer, unsigned short);
Result write(C* c, Writer writer, int);
Result write(C* c, Writer writer, unsigned int);
Result write(C* c, Writer writer, long);
Result write(C* c, Writer writer, unsigned long);
Result write(C* c, Writer writer, long long);
Result write(C* c, Writer writer, unsigned long long);

struct Address {
    void* val;
};

inline Address addr(void* val) {
    return {val};
}

Result write(C* c, Writer writer, Address addr);

template <class T>
struct Debug {
    T val;
};

template <class T>
inline Debug<T> debug(T val) {
    return {val};
}

template <class T>
Result write(C* c, Writer writer, Debug<T> debug) {
    return write(c, writer, debug.val);
}

template <class T>
Result write(C* c, Writer writer, Debug<Slice<T>> debug_slice) {
    auto slice = debug_slice.val;
    CZ_TRY(write(c, writer, '['));

    for (size_t i = 0; i < slice.len; ++i) {
        if (i != 0) {
            CZ_TRY(write(c, writer, ", "));
        }

        CZ_TRY(write(c, writer, debug(slice.buffer[i])));
    }

    return write(c, writer, ']');
}

}
}
