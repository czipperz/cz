#include "assert.hpp"

#include <stdlib.h>
#include "logger.hpp"

namespace cz {

void assert_(SourceLocation loc, const char* expression_string, bool value) {
    if (!value) {
        io::write(log::fatal(), loc.file, ':', loc.line, ": Assertion failed: '", expression_string,
                  '\'');
        abort();
    }
}

void panic_(SourceLocation loc, const char* message) {
    io::write(log::fatal(), loc.file, ':', loc.line, ": ", message);
    abort();
}

}
