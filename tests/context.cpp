#include "context.hpp"

#include "../src/context.hpp"
#include "../src/logger.hpp"

namespace cz {
namespace test {

Context ctxt(mem::Allocator allocator) {
    return {allocator, NULL, log::ignore(), log::LogLevel::Off};
}

}
}
