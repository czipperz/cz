#pragma once

#include "context_decl.hpp"
#include "logger_decl.hpp"
#include "mem/allocator.hpp"

namespace cz {

struct Context {
    mem::Allocator allocator;

    log::Logger logger;
    log::LogLevel max_log_level;

    /// Allocate memory using this allocator.
    MemSlice alloc(mem::AllocInfo new_info) const { return allocator.alloc(this, new_info); }

    /// Allocate memory to store a value of the given type.
    template <class T>
    T* alloc() const {
        return allocator.alloc<T>(this);
    }

    /// Deallocate memory allocated using this allocator.
    void dealloc(MemSlice mem) const { return allocator.dealloc(this, mem); }

    /// Reallocate memory allocated using this allocator.
    MemSlice realloc(MemSlice old_mem, mem::AllocInfo new_info) const {
        return allocator.realloc(this, old_mem, new_info);
    }

    /// Log the following information.
    void log(log::LogInfo info) const {
        if (info.level <= max_log_level) {
            return logger.log(this, info);
        } else {
            return;
        }
    }
};

}
