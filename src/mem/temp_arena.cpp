#include <cz/mem/temp_arena.hpp>

#include <cz/util.hpp>

namespace cz {
namespace mem {

static void set_max_result(TempArena* ta) {
    ta->max_offset = max(ta->max_offset, ta->arena.offset);
}

static MemSlice temp_arena_alloc(void* _ta, AllocInfo info) {
    auto ta = static_cast<TempArena*>(_ta);
    auto result = ta->arena.allocator().alloc(info);
    set_max_result(ta);
    return result;
}

static void temp_arena_dealloc(void* _ta, MemSlice old_mem) {
    auto ta = static_cast<TempArena*>(_ta);
    ta->arena.allocator().dealloc(old_mem);
}

static MemSlice temp_arena_realloc(void* _ta, MemSlice old_mem, AllocInfo new_info) {
    auto ta = static_cast<TempArena*>(_ta);
    auto result = ta->arena.allocator().realloc(old_mem, new_info);
    set_max_result(ta);
    return result;
}

Allocator TempArena::allocator() {
    static const Allocator::VTable vtable = {temp_arena_alloc, temp_arena_dealloc,
                                             temp_arena_realloc};
    return {&vtable, this};
}

}
}
