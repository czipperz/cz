#pragma once

#define CZ_SOURCE_LOCATION (cz::SourceLocation { __FILE__, __LINE__ })

namespace cz {

struct SourceLocation {
    const char* file;
    long line;
};

}
