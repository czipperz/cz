#pragma once

#include <string>
#include "write.hpp"

namespace cz {

Writer std_string_writer(std::string*);

template <class T>
std::string sprint_std(T t) {
    std::string string;
    write(std_string_writer(&string), t);
    return string;
}

}
