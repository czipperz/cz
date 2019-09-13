#include <cz/logger.hpp>

namespace cz {

Logger Logger::ignore() {
    static const Logger::VTable vtable = {
        [](void*, const LogInfo&) { return Result::ok(); },
        [](void*, const LogInfo&, Str) { return Result::ok(); },
        [](void*, const LogInfo&) { return Result::ok(); }};
    return {&vtable, nullptr};
}

}
