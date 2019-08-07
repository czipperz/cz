#include "context.hpp"

#include "assert.hpp"

namespace cz {

static MemSlice temp_alloc(C* c, void*, mem::AllocInfo info) {
    CZ_DEBUG_ASSERT(c != NULL);
    CZ_DEBUG_ASSERT(c->temp != NULL);
    return c->temp->allocator().alloc(c, info);
}

static void temp_dealloc(C* c, void*, MemSlice mem) {
    CZ_DEBUG_ASSERT(c != NULL);
    CZ_DEBUG_ASSERT(c->temp != NULL);
    c->temp->allocator().dealloc(c, mem);
}

static MemSlice temp_realloc(C* c, void*, MemSlice old_mem, mem::AllocInfo new_info) {
    CZ_DEBUG_ASSERT(c != NULL);
    CZ_DEBUG_ASSERT(c->temp != NULL);
    return c->temp->allocator().realloc(c, old_mem, new_info);
}

mem::Allocator temp_allocator() {
    return {{temp_alloc, temp_dealloc, temp_realloc}, NULL};
}

}
