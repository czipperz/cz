#pragma once

#include "allocator.hpp"

namespace cz {
namespace mem {

struct MultiArena {
    struct Node {
        Node* next;
        size_t offset;
        size_t size;
    };
    Node* head = NULL;

    Allocator inner_allocator;

    constexpr explicit MultiArena(Allocator inner_allocator) : inner_allocator(inner_allocator) {}
    void drop();

    Allocator allocator();
};

}
}
