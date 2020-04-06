#pragma once

#include <exception>
#include "source_location.hpp"
#include "stringify.hpp"

#ifdef NDEBUG
#define CZ_DEBUG_ASSERT(ignore) ((void)0)
#else
#define CZ_DEBUG_ASSERT(val) CZ_ASSERT(val)
#endif

#define CZ_ASSERT(val) ((val) ? ((void)0) : CZ_PANIC("Assertion failed: " CZ_STRINGIFY(val)))

#define CZ_PANIC(message) \
    (cz::impl::panic_reached(__FILE__ ":" CZ_STRINGIFY(__LINE__) ": " message))

namespace cz {

struct PanicReachedException : std::exception {
    const char* message;

    PanicReachedException(const char* message);

    virtual const char* what() const noexcept override;
};

namespace impl {
[[noreturn]] void panic_reached(const char* message);
}

}
