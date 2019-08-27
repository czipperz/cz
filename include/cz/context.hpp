#pragma once

#include "context_decl.hpp"
#include "logger_decl.hpp"
#include "mem.hpp"

namespace cz {

struct Context {
    mem::Allocator allocator;
    mem::TempArena* temp;

    log::Logger logger;
    log::LogLevel max_log_level;
};

}
