#pragma once

#include "string.hpp"
#include "io/write.hpp"
#include "context.hpp"

namespace cz {
namespace format {

template <class... Ts>
String sprint(C* c, Ts... ts) {
    String string;
    io::write(c, io::string_writer(&string), ts...);
    return string;
}

}
}
