#include "result.hpp"

#include <errno.h>

namespace cz {
namespace io {

Result Result::last_error() {
    return {errno};
}

}
}
