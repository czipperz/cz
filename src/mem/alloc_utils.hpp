#pragma once

#include "../slice.hpp"
#include "alloc_info.hpp"

namespace cz {
namespace mem {

/// If \c info can be allocated in \c mem, return the aligned pointer.
/// Otherwise returns NULL.
void* advance_ptr_to_alignment(MemSlice mem, AllocInfo info);

}
}
