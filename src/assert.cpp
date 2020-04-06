#include <cz/assert.hpp>

#include <stdlib.h>

namespace cz {

PanicReachedException::PanicReachedException(const char* message) : message(message) {}

const char* PanicReachedException::what() const noexcept {
    return message;
}

namespace impl {

void panic_reached(const char* message) {
    throw PanicReachedException(message);
}

}
}
