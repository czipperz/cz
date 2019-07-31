#include "global_allocator.hpp"

#include "heap.hpp"

namespace cz {
namespace mem {

Allocator global_allocator = heap::allocator();

}
}
