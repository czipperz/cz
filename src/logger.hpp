#pragma once

#include "context.hpp"
#include "defer.hpp"
#include "format.hpp"
#include "logger_decl.hpp"

namespace cz {
namespace log {

template <LogLevel level, class... Ts>
void log(C* c, const char* file, size_t line, Ts... ts) {
    String message = cz::format::sprint(c->temp, ts...);
    message.realloc(c->temp);
    CZ_DEFER(message.drop(c->temp));
    c->log(LogInfo(file, line, level, message));
}

#define CZ_LOG(c, level, ...) \
    (cz::log::log<cz::log::LogLevel::level>(c, __FILE__, __LINE__, __VA_ARGS__))

struct BasicLogger {
    LogFormatter formatter;
    io::Writer out;

    static void write(C* c, void* _self, LogInfo info) {
        auto self = static_cast<BasicLogger*>(_self);
        self->formatter.write_to(c, self->out, info);
    }

    explicit operator Logger() { return {write, this}; }
};

inline Logger ignore() {
    return {[](C*, void*, LogInfo) {}, NULL};
}

}
}
