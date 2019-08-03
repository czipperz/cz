#include "result.hpp"

#include <string.h>
#include "../mem.hpp"

namespace cz {
namespace io {

Result Result::from_errno(int e) {
    Str message = strerror(e);
    auto buffer = static_cast<char*>(mem::global_allocator.alloc(message.len));
    memcpy(buffer, message.buffer, message.len);
    return err(buffer);
}

}
}
