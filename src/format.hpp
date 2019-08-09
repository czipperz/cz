#pragma once

#include "string.hpp"
#include "io/write.hpp"
#include "context.hpp"

namespace cz {
namespace format {

template <class... Ts>
String sprint(mem::Allocator allocator, Ts... ts) {
    mem::Allocated<String> string(allocator);
    io::write(io::string_writer(&string), ts...);
    return string;
}

}
}
