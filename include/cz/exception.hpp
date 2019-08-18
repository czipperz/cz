#pragma once

#include "context_decl.hpp"

namespace cz {

struct Exception {
    virtual void log(C*) = 0;
};

}
