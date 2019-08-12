#pragma once

#include <string>
#include "write.hpp"

namespace cz {
namespace io {

Writer sstring_writer(std::string*);

template <class T>
std::string ssprint(T t) {
    std::string string;
    write(sstring_writer(&string), t);
    return string;
}

}
}
