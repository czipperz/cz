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

    Allocator allocator();
    void drop(C* c);
};

}
}
