#pragma once

#include "context.hpp"
#include "string.hpp"
#include "write.hpp"

namespace cz {

template <class... Ts>
String sprint(Allocator allocator, Ts... ts) {
    AllocatedString string;
    string.allocator = allocator;
    write(string_writer(&string), ts...);
    return /* slice */ string;
}

namespace format {

struct Address {
    void* val;
};

inline Address addr(void* val) {
    return {val};
}

template <class T>
struct WidthSpecified {
    T val;
    size_t width;
};

template <class T>
inline WidthSpecified<T> width(size_t width, T val) {
    return {val, width};
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

Result write(Writer writer, format::Address addr);

Result write(Writer writer, format::WidthSpecified<short>);
Result write(Writer writer, format::WidthSpecified<unsigned short>);
Result write(Writer writer, format::WidthSpecified<int>);
Result write(Writer writer, format::WidthSpecified<unsigned int>);
Result write(Writer writer, format::WidthSpecified<long>);
Result write(Writer writer, format::WidthSpecified<unsigned long>);
Result write(Writer writer, format::WidthSpecified<long long>);
Result write(Writer writer, format::WidthSpecified<unsigned long long>);

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
Result write(Writer writer, format::Debug<AllocInfo>);

template <class T>
Result write(Writer writer, format::Debug<Slice<T>> debug_slice) {
    auto slice = debug_slice.val;
    CZ_TRY(write(writer, '['));

    for (size_t i = 0; i < slice.len; ++i) {
        if (i != 0) {
            CZ_TRY(write(writer, ", "));
        }

        CZ_TRY(write(writer, format::debug(slice[i])));
    }

    return write(writer, ']');
}

}
