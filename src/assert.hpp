#pragma once

#include "context.hpp"
#include "source_location.hpp"
#include "stringify.hpp"

#define CZ_DEBUG_ASSERT(c, val) CZ_ASSERT(c, val)

#ifdef NDEBUG
#define CZ_ASSERT(c, ignore) ((void)0)
#else
#define CZ_ASSERT(c, val) \
    ((val) ? ((void)0) : cz::impl::assert_fail(c, CZ_SOURCE_LOCATION, CZ_STRINGIFY(val)))
#endif

#define CZ_PANIC(c, message) (cz::impl::panic_(c, CZ_SOURCE_LOCATION, message))

namespace cz {
namespace impl {

[[noreturn]] void assert_fail(C* c, SourceLocation, const char* expression_string);

[[noreturn]] void panic_(C* c, SourceLocation, const char* message);

}
}
