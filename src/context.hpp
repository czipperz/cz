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
    void* alloc(mem::AllocInfo new_info) { return allocator.alloc(this, new_info); }

    /// Allocate memory to store a value of the given type.
    template <class T>
    T* alloc() {
        return allocator.alloc<T>(this);
    }

    /// Deallocate memory allocated using this allocator.
    void dealloc(MemSlice mem) { return allocator.dealloc(this, mem); }

    /// Reallocate memory allocated using this allocator.
    void* realloc(MemSlice old_mem, mem::AllocInfo new_info) {
        return allocator.realloc(this, old_mem, new_info);
    }

    /// Reallocate memory alloceted using this allocator to store a value of the
    /// given type.
    template <class T>
    T* realloc(void* old_ptr, size_t old_size) {
        return allocator.realloc<T>(this, old_ptr, old_size);
    }

    /// Log the following information.
    void log(log::LogInfo info) {
        if (info.level <= max_log_level) {
            return logger.log(this, info);
        } else {
            return;
        }
    }
};

}
