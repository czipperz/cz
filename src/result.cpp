#include <cz/result.hpp>

#include <errno.h>

namespace cz {

Result Result::last_error() {
    return {errno};
}

}
