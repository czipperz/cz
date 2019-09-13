#pragma once

#include "context_decl.hpp"
#include "logger_decl.hpp"
#include "allocator.hpp"
#include "temp_arena.hpp"

namespace cz {

struct Context {
    Allocator allocator;
    TempArena* temp;

    log::Logger logger;
    log::LogLevel max_log_level;
};

}
