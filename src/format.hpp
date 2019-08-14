#pragma once

#include "string.hpp"
#include "io/write.hpp"
#include "context.hpp"

namespace cz {
namespace format {

template <class... Ts>
String sprint(mem::Allocator allocator, Ts... ts) {
    mem::Allocated<String> string(allocator);
    io::write(io::string_writer(&string), ts...);
    return string;
}

struct Address {
    void* val;
};

inline Address addr(void* val) {
    return {val};
}

template <class T>
struct Debug {
    T val;
};

template <class T>
inline Debug<T> debug(T val) {
    return {val};
}
}

namespace io {
Result write(Writer writer, format::Address addr);

template <class T>
Result write(Writer writer, format::Debug<T> debug) {
    return write(writer, debug.val);
}

Result write(Writer writer, format::Debug<Str>);
inline Result write(Writer writer, format::Debug<const char*> str) {
    return write(writer, format::debug(Str(str.val)));
}
inline Result write(Writer writer, format::Debug<String> string) {
    return write(writer, format::debug(Str(string.val)));
}

Result write(Writer writer, format::Debug<MemSlice>);
Result write(Writer writer, format::Debug<mem::AllocInfo>);

template <class T>
Result write(Writer writer, format::Debug<Slice<T>> debug_slice) {
    auto slice = debug_slice.val;
    CZ_TRY(write(writer, '['));

    for (size_t i = 0; i < slice.len; ++i) {
        if (i != 0) {
            CZ_TRY(write(writer, ", "));
        }

        CZ_TRY(write(writer, format::debug(slice.buffer[i])));
    }

    return write(writer, ']');
}

}
}
