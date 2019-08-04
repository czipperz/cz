#include "multi_arena.hpp"

#include "../assert.hpp"
#include "../util.hpp"
#include "alloc_utils.hpp"
#include "arena.hpp"

namespace cz {
namespace mem {

static Arena node_arena(MultiArena::Node* node) {
    Arena arena({node + 1, node->size});
    arena.offset = node->offset;
    return arena;
}

static MemSlice node_alloc(C* c, MultiArena* multi_arena, AllocInfo new_info) {
    auto arena = node_arena(multi_arena->head);
    auto mem = arena.allocator().alloc(c, new_info);
    if (mem.buffer) {
        multi_arena->head->offset = arena.offset;
        return mem;
    }
    return {NULL, 0};
}

static MemSlice multi_arena_allocate(C* c,
                                     void* _multi_arena,
                                     MemSlice old_mem,
                                     AllocInfo new_info) {
    auto old_aligned = advance_ptr_to_alignment(old_mem, new_info);
    if (old_aligned) {
        return {old_aligned, new_info.size};
    }

    auto multi_arena = static_cast<MultiArena*>(_multi_arena);
    if (multi_arena->head) {
        auto res = node_alloc(c, multi_arena, new_info);
        if (res.buffer) {
            return res;
        }
    }

    auto size = max((size_t)1024, sizeof(MultiArena::Node) + new_info.size);
    auto new_head = static_cast<MultiArena::Node*>(c->alloc(size).buffer);
    CZ_DEBUG_ASSERT(c, new_head != NULL);
    new_head->next = multi_arena->head;
    new_head->offset = 0;
    new_head->size = size;
    multi_arena->head = new_head;

    return node_alloc(c, multi_arena, new_info);
}

Allocator MultiArena::allocator() {
    return {
        multi_arena_allocate,
        this,
    };
}

void MultiArena::drop(C* c) {
    auto node = head;
    while (node) {
        auto next = node->next;
        c->dealloc({node, node->size});
        node = next;
    }
}

}
}
