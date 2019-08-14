#include "str.hpp"

#include "assert.hpp"
#include "context.hpp"
#include "string.hpp"

namespace cz {

String Str::duplicate(mem::Allocator allocator) const {
    auto ptr = static_cast<char*>(allocator.alloc(len).buffer);
    CZ_ASSERT(ptr != nullptr);
    memcpy(ptr, buffer, len);
    return String{ptr, len, len};
}

}
