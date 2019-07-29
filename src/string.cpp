#include "string.hpp"

#include <string.h>
#include "mem.hpp"

namespace cz {

Str Str::from(const char* str) {
    return {str, strlen(str)};
}

Str String::as_str() {
    return {start, len};
}

String::String() : start(0), len(0), cap(0) {}
String::String(char* buffer, size_t len) : start(buffer), len(len), cap(len) {}

void String::drop() {
    mem::dealloc(start, cap);
}

}
