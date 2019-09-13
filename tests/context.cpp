#include "context.hpp"

#include <cz/context.hpp>
#include <cz/logger.hpp>
#include <czt/mock_allocate.hpp>

namespace cz {
namespace test {

Context ctxt(Allocator allocator) {
    return {allocator, {}, Logger::ignore(), LogLevel::Off};
}

}
}
