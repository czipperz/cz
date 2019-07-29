#pragma once

#include "source_location.hpp"
#include "stringify.hpp"

#ifdef NDEBUG
#define CZ_DEBUG_ASSERT(ignore) ((void)0)
#else
#define CZ_DEBUG_ASSERT(val) CZ_ASSERT(val)
#endif

#define CZ_ASSERT(val) cz::assert_(CZ_SOURCE_LOCATION, CZ_STRINGIFY(val), (val))

namespace cz {
void assert_(SourceLocation, const char* expression_string, bool value);
}
