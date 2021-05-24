#include <cz/debug.hpp>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#else
#include <signal.h>
#endif

namespace cz {

void dbreak() {
#ifdef _WIN32
    if (IsDebuggerPresent()) {
        DebugBreak();
    }
#else
    // On Linux there isn't a direct way to check if a debugger is present and then
    // break.  So instead we'll handle the case where a debugger isn't present by
    // masking SIGTRAP and then raising a SIGTRAP.  If a debugger has been installed
    // then SIGTRAP is handled specially so we won't hit our custom signal handler.
    signal(SIGTRAP, [](int) {});
    raise(SIGTRAP);
    signal(SIGTRAP, SIG_DFL);
#endif
}

void cbreak(bool condition) {
    if (condition) {
        dbreak();
    }
}

}
