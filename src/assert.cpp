#include "assert.hpp"

#include <stdlib.h>
#include "logger.hpp"

namespace cz {

void assert_(C* c, SourceLocation loc, const char* expression_string, bool value) {
    if (!value) {
        log::fatal(c, loc.file, ':', loc.line, ": Assertion failed: '",
                  expression_string, "'\n");
        abort();
    }
}

void panic_(C* c, SourceLocation loc, const char* message) {
    log::fatal(c, loc.file, ':', loc.line, ": ", message, '\n');
    abort();
}

}
