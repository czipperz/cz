#include "assert.hpp"

#include <stdio.h>
#include <stdlib.h>

namespace cz {

void assert_(SourceLocation loc, const char* expression_string, bool value) {
    if (!value) {
        fprintf(stderr, "%s:%u: Assertion failed: '%s'", loc.file, loc.line, expression_string);
        abort();
    }
}

}
