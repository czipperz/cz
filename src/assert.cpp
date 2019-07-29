#include "assert.hpp"

#include <stdio.h>

namespace cz {

void assert_(SourceLocation loc, const char* expression_string, bool value) {
    if (!value) {
        printf("%s:%u: Assertion failed: '%s'", loc.file, loc.line, expression_string);
    }
}

}
