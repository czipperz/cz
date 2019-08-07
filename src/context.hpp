#pragma once

#include "context_decl.hpp"
#include "logger_decl.hpp"
#include "mem/allocator.hpp"
#include "mem/arena.hpp"

namespace cz {

struct Context {
    mem::Allocator allocator;
    mem::Arena* temp;

    log::Logger logger;
    log::LogLevel max_log_level;

    /// Allocate memory using the allocator.
    MemSlice alloc(mem::AllocInfo info) const { return allocator.alloc(this, info); }
    /// Allocate memory using the temporary allocator.
    MemSlice talloc(mem::AllocInfo info) const { return temp->allocator().alloc(this, info); }

    /// Allocate memory to store a value of the given type using the allocator.
    template <class T>
    T* alloc() const {
        return allocator.alloc<T>(this);
    }
    /// Allocate memory to store a value of the given type using the temporary allocator.
    template <class T>
    T* talloc() const {
        return temp->allocator().alloc<T>(this);
    }

    /// Deallocate memory allocated using the allocator.
    void dealloc(MemSlice mem) const { return allocator.dealloc(this, mem); }
    /// Deallocate memory allocated using the temporary allocator.
    void tdealloc(MemSlice mem) const { return temp->allocator().dealloc(this, mem); }

    /// Reallocate a section of memory allocated using the allocator.
    MemSlice realloc(MemSlice old_mem, mem::AllocInfo new_info) const {
        return allocator.realloc(this, old_mem, new_info);
    }
    /// Reallocate a section of memory allocated using the temporary allocator.
    MemSlice trealloc(MemSlice old_mem, mem::AllocInfo new_info) const {
        return temp->allocator().realloc(this, old_mem, new_info);
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
