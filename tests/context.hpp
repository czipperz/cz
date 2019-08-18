#include <cz/context.hpp>

namespace cz {

namespace mem {
struct Allocator;
}

namespace test {

Context ctxt(mem::Allocator allocator);

}
}
