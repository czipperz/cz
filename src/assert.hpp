#pragma once

#include "exception.hpp"
#include "source_location.hpp"
#include "stringify.hpp"

#ifdef NDEBUG
#define CZ_DEBUG_ASSERT(ignore) ((void)0)
#else
#define CZ_DEBUG_ASSERT(val) CZ_ASSERT(val)
#endif

#define CZ_ASSERT(val) ((val) ? ((void)0) : CZ_PANIC("Assertion failed: " CZ_STRINGIFY(val)))

#define CZ_PANIC(message) (cz::impl::panic_reached(CZ_SOURCE_LOCATION, message))

namespace cz {

struct PanicReachedException : Exception {
    SourceLocation loc;
    const char* message;

    PanicReachedException(SourceLocation loc, const char* message);

    virtual void log(C* c) override;
};

namespace impl {

[[noreturn]] void panic_reached(SourceLocation, const char* message);

}
}
