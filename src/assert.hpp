#pragma once

#include "context.hpp"
#include "source_location.hpp"
#include "stringify.hpp"

#ifdef NDEBUG
#define CZ_DEBUG_ASSERT(c, ignore) ((void)0)
#else
#define CZ_DEBUG_ASSERT(c, val) CZ_ASSERT(c, val)
#endif

#define CZ_ASSERT(c, val) cz::assert_(c, CZ_SOURCE_LOCATION, CZ_STRINGIFY(val), (val))

#define CZ_PANIC(c, message) cz::panic_(c, CZ_SOURCE_LOCATION, message)

namespace cz {

void assert_(C* c, SourceLocation, const char* expression_string, bool value);

void panic_(C* c, SourceLocation, const char* message);

}
