#include "cz/assert.hpp"

#include <stdlib.h>
#include "cz/logger.hpp"

namespace cz {

PanicReachedException::PanicReachedException(SourceLocation loc, const char* message)
    : loc(loc), message(message) {}

void PanicReachedException::log(C* c) {
    CZ_LOG(c, Fatal, loc.file, ':', loc.line, ": ", message, '\n');
}

const char* PanicReachedException::what() const noexcept {
    return message;
}

namespace impl {

void panic_reached(SourceLocation loc, const char* message) {
    throw PanicReachedException(loc, message);
}

}
}
