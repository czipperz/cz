#pragma once

#include "alloc_info.hpp"
#include "slice.hpp"

namespace cz {

/// If \c info can be allocated in \c mem, return the aligned pointer.
/// Otherwise returns nullptr.
void* advance_ptr_to_alignment(MemSlice mem, AllocInfo info);

}
