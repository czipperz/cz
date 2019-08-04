#pragma once

#include "context.hpp"
#include "defer.hpp"
#include "logger_decl.hpp"

namespace cz {
namespace log {

template <LogLevel level, class... Ts>
void log(C* c, Ts... ts) {
    String message = concat(c, ts...);
    CZ_DEFER(message.drop(c));
    c->log(LogInfo(level, message));
}

template <class... Ts>
void fatal(C* c, Ts... ts) {
    return log<LogLevel::Fatal>(c, ts...);
}
template <class... Ts>
void error(C* c, Ts... ts) {
    return log<LogLevel::Error>(c, ts...);
}
template <class... Ts>
void warning(C* c, Ts... ts) {
    return log<LogLevel::Warning>(c, ts...);
}
template <class... Ts>
void important(C* c, Ts... ts) {
    return log<LogLevel::Important>(c, ts...);
}
template <class... Ts>
void information(C* c, Ts... ts) {
    return log<LogLevel::Information>(c, ts...);
}
template <class... Ts>
void debug(C* c, Ts... ts) {
    return log<LogLevel::Debug>(c, ts...);
}
template <class... Ts>
void trace(C* c, Ts... ts) {
    return log<LogLevel::Trace>(c, ts...);
}

}
}
