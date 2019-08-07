#include "context.hpp"

#include "../src/context.hpp"
#include "../src/logger.hpp"
#include "mem/mock_allocate.hpp"

namespace cz {
namespace test {

Context ctxt(mem::Allocator allocator) {
    return {allocator, panic_allocator(), log::ignore(), log::LogLevel::Off};
}

}
}
