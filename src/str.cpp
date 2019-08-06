#include "str.hpp"

#include "assert.hpp"
#include "context.hpp"
#include "string.hpp"

namespace cz {

String Str::duplicate(C* c) const {
    auto ptr = static_cast<char*>(c->alloc(len).buffer);
    CZ_ASSERT(ptr != NULL);
    memcpy(ptr, buffer, len);
    return String{ptr, len, len};
}

}
