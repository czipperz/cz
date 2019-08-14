#pragma once

#include "context.hpp"
#include "defer.hpp"
#include "format.hpp"
#include "logger_decl.hpp"

namespace cz {
namespace log {

template <class... Ts>
void log(C* c, LogLevel level, const char* file, size_t line, Ts... ts) {
    String message = cz::format::sprint(c->temp, ts...);
    message.realloc(c->temp);
    CZ_DEFER(message.drop(c->temp));
    c->log(LogInfo(file, line, level, message));
}

#define CZ_LOG(c, level, ...) (CZ_LOGL(c, cz::log::LogLevel::level, __VA_ARGS__))
#define CZ_LOGL(c, level, ...) (cz::log::log(c, level, __FILE__, __LINE__, __VA_ARGS__))

Logger ignore();

}

}
