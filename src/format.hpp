#include "string.hpp"

namespace cz {
namespace format {

enum class Result {
    Ok = 0,
    NotFound,
    PermissionDenied,
    ConnectionRefused,
    ConnectionReset,
    ConnectionAborted,
    NotConnected,
    AddrInUse,
    AddrNotAvailable,
    BrokenPipe,
    AlreadyExists,
    WouldBlock,
    InvalidData,
    TimedOut,
    WriteZero,
    Interrupted,
    UnexpectedEof,
    Other,
};

struct Write {
    Result (*write_str)(void* data, Str str);
};

struct Writer {
    Write write;
    void* data;

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

Result write(Writer writer, const char*);
Result write(Writer writer, Str);

}
}
