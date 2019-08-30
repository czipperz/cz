#include <cz/format.hpp>

namespace cz {

using namespace format;

Result write(Writer writer, Debug<Str> str) {
    CZ_TRY(write(writer, '"'));
    for (size_t i = 0; i < str.val.len; ++i) {
        char ch = str.val[i];
        if (ch == '\\') {
            CZ_TRY(write(writer, "\\\\"));
        } else if (ch == '"') {
            CZ_TRY(write(writer, "\\\""));
        } else if (ch == '\n') {
            CZ_TRY(write(writer, "\\n"));
        } else {
            CZ_TRY(write(writer, ch));
        }
    }
    return write(writer, '"');
}

Result write(Writer writer, Debug<MemSlice> slice) {
    return write(writer, "MemSlice{ ", addr(slice.val.buffer), ", ", slice.val.size, " }");
}
Result write(Writer writer, Debug<mem::AllocInfo> info) {
    return write(writer, "AllocInfo{ ", info.val.size, ", ", info.val.alignment, " }");
}

}
