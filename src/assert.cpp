#include "assert.hpp"

#include <stdlib.h>
#include "logger.hpp"

namespace cz {
namespace impl {

void assert_fail(C* c, SourceLocation loc, const char* expression_string) {
    log::fatal(c, loc.file, ':', loc.line, ": Assertion failed: '", expression_string, "'\n");
    abort();
}

void panic_(C* c, SourceLocation loc, const char* message) {
    log::fatal(c, loc.file, ':', loc.line, ": ", message, '\n');
    abort();
}

}
}
