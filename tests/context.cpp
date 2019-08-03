#include "context.hpp"

#include "../src/context.hpp"

namespace cz {
namespace test {

Context ctxt(mem::Allocator allocator) {
    return {allocator, log::ignore(), log::LogLevel::Off};
}

}
}
