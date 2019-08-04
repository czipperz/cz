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

static MemSlice node_alloc(C* c, MultiArena::Node* node, AllocInfo new_info) {
    auto arena = node_arena(node);

    MemSlice mem = arena.allocator().alloc(c, new_info);
    if (mem.buffer) {
        node->offset = arena.offset;
    }
    return mem;
}

static MemSlice node_realloc(C* c, MultiArena::Node* node, MemSlice old_mem, AllocInfo new_info) {
    auto arena = node_arena(node);

    MemSlice mem;
    if (arena.mem.contains(old_mem)) {
        mem = arena.allocator().realloc(c, old_mem, new_info);
    } else {
        mem = arena.allocator().alloc(c, new_info);
    }

    if (mem.buffer) {
        node->offset = arena.offset;
    }

    return mem;
}

static bool push_new_node(C* c, MultiArena* multi_arena, AllocInfo info) {
    auto size = max((size_t)1024, sizeof(MultiArena::Node) + info.size);
    auto new_head = static_cast<MultiArena::Node*>(c->alloc(size).buffer);
    if (new_head) {
        new_head->next = multi_arena->head;
        new_head->offset = 0;
        new_head->size = size;
        multi_arena->head = new_head;
    }
    return new_head;
}

static MemSlice multi_arena_alloc(C* c, void* _multi_arena, AllocInfo info) {
    auto multi_arena = static_cast<MultiArena*>(_multi_arena);
    if (multi_arena->head) {
        auto res = node_alloc(c, multi_arena->head, info);
        if (res.buffer) {
            return res;
        }
    }

    if (push_new_node(c, multi_arena, info)) {
        return node_alloc(c, multi_arena->head, info);
    } else {
        return {NULL, 0};
    }
}

static void multi_arena_dealloc(C* c, void* _multi_arena, MemSlice mem) {
    auto multi_arena = static_cast<MultiArena*>(_multi_arena);
    // Only actually deallocate if the memory is in the head node (ie the most recent allocation).
    if (multi_arena->head) {
        auto arena = node_arena(multi_arena->head);
        if (arena.mem.contains(mem)) {
            arena.allocator().dealloc(c, mem);
        }
    }
}

static MemSlice multi_arena_realloc(C* c,
                                    void* _multi_arena,
                                    MemSlice old_mem,
                                    AllocInfo new_info) {
    auto old_aligned = advance_ptr_to_alignment(old_mem, new_info);
    if (old_aligned) {
        return {old_aligned, new_info.size};
    }

    auto multi_arena = static_cast<MultiArena*>(_multi_arena);
    if (multi_arena->head) {
        auto res = node_realloc(c, multi_arena->head, old_mem, new_info);
        if (res.buffer) {
            return res;
        }
    }

    auto new_mem = multi_arena_alloc(c, multi_arena, new_info);
    if (new_mem.buffer) {
        memcpy(new_mem.buffer, old_mem.buffer, min(new_mem.size, old_mem.size));
    }
    return new_mem;
}

Allocator MultiArena::allocator() {
    return {
        {multi_arena_alloc, multi_arena_dealloc, multi_arena_realloc},
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
