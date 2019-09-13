#include <cz/alloc_utils.hpp>

#include <memory>

namespace cz {

void* advance_ptr_to_alignment(MemSlice mem, AllocInfo info) {
    // Note that std::align uses references to modify mem inplace
    return std::align(info.alignment, info.size, mem.buffer, mem.size);
}

}
