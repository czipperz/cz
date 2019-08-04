#include "result.hpp"

#include <string.h>
#include "../assert.hpp"
#include "../context.hpp"
#include "../mem.hpp"

namespace cz {
namespace io {

Result Result::from_errno(C* c, int e) {
    Str message = strerror(e);
    auto mem = c->alloc(message.len);
    memcpy(mem.buffer, message.buffer, message.len);
    CZ_PANIC(c, "Need to design errors");
    //return err(buffer);
    return ok();
}

}
}
