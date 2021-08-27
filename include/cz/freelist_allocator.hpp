#pragma once

#include <cz/heap.hpp>

namespace cz {

struct Freelist_Node {
    Freelist_Node* next;
};

struct Freelist_Common {
    Freelist_Node* head;

// #ifndef NDEBUG
    size_t list_size;
// #endif

    void drop(Allocator allocator);
};

/// An allocator that holds onto freed elements and reuses them.
/// If no freed element is available then allocates using the `backer` allocator.
/// All allocations must be the same size.
struct Freelist {
    // Fields.  Common must be first.
    Freelist_Common common;
    Allocator backer;

    // Methods
    Allocator allocator() { return {realloc, dealloc, this}; }
    void drop() { common.drop(backer); }

private:
    static void* realloc(void* freelist, MemSlice old_mem, AllocInfo new_info);
    static void dealloc(void* freelist, MemSlice old_mem);
    friend struct Freelist_Heap;
};

/// An allocator that holds onto freed elements and reuses them.
/// If no freed element is available then allocates using the `heap_allocator`.
/// All allocations must be the same size.
struct Freelist_Heap {
    // Fields.  Common must be first.
    Freelist_Common common;

    // Methods
    Allocator allocator() { return {realloc, Freelist::dealloc, this}; }
    void drop() { common.drop(heap_allocator()); }

private:
    static void* realloc(void* freelist, MemSlice old_mem, AllocInfo new_info);
};

}
