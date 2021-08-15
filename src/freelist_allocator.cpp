#include <cz/freelist_allocator.hpp>

#include <cz/util.hpp>

namespace cz {

void Freelist_Common::drop(Allocator allocator) {
    Freelist_Node* node = head;
    while (node) {
        Freelist_Node* next = node->next;
        allocator.dealloc({node, list_size});
        node = next;
    }
}

static void* freelist_realloc(void* freelist_,
                              Allocator backer,
                              MemSlice old_mem,
                              AllocInfo new_info) {
    Freelist* freelist = (Freelist*)freelist_;

    new_info.size = cz::max(new_info.size, sizeof(Freelist_Node));

#ifndef NDEBUG
    // Cannot realloc because all allocations must be the same size.
    if (old_mem.buffer != nullptr) {
        CZ_PANIC("Freelist cannot realloc");
    }
#endif

    if (freelist->common.head) {
        // Check that size requested and size provided are equal.
#ifndef NDEBUG
        if (new_info.size != freelist->common.list_size) {
            CZ_PANIC("Freelist requires all allocations are of the same size");
        }
#endif

        // Pop node.
        Freelist_Node* node = freelist->common.head;
        freelist->common.head = node->next;
        return node;
    }

    return backer.realloc(old_mem, new_info);
}

void* Freelist::realloc(void* freelist_, MemSlice old_mem, AllocInfo new_info) {
    return freelist_realloc(freelist_, ((Freelist*)freelist_)->backer, old_mem, new_info);
}

void* Freelist_Heap::realloc(void* freelist_, MemSlice old_mem, AllocInfo new_info) {
    return freelist_realloc(freelist_, heap_allocator(), old_mem, new_info);
}

void Freelist::dealloc(void* freelist_, MemSlice old_mem) {
    if (!old_mem.buffer) {
        return;
    }

    Freelist_Common* freelist_common = (Freelist_Common*)freelist_;

    old_mem.size = cz::max(old_mem.size, sizeof(Freelist_Node));

    if (freelist_common->head) {
#ifndef NDEBUG
        if (old_mem.size != freelist_common->list_size) {
            CZ_PANIC("Freelist requires all allocations are of the same size");
        }
#endif
    } else {
        freelist_common->list_size = old_mem.size;
    }

    CZ_DEBUG_ASSERT(old_mem.buffer != nullptr);

    Freelist_Node* node = (Freelist_Node*)old_mem.buffer;
    node->next = freelist_common->head;
    freelist_common->head = node;
}

}
