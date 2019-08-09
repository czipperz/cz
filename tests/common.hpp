#pragma once

#define CATCH_CONFIG_FALLBACK_STRINGIFIER(value) (cz::io::ssprint(cz::io::debug(value)))

#include <string>
#include "../src/io/write.hpp"

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

#include "catch.hpp"
