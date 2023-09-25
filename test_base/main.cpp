#define CATCH_CONFIG_MAIN
#include "czt/test_base.hpp"

#include <cz/string.hpp>

namespace Catch {

std::string StringMaker<cz::String>::convert(const cz::String& str) {
    std::string std_string;
    std_string.assign(str.buffer, str.len);
    return std_string;
}

std::string StringMaker<cz::Str>::convert(const cz::Str& str) {
    std::string std_string;
    std_string.assign(str.buffer, str.len);
    return std_string;
}

}
