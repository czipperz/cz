#include "context.hpp"

#include <cz/context.hpp>
#include <cz/logger.hpp>
#include "mem/mock_allocate.hpp"

namespace cz {
namespace test {

Context ctxt(mem::Allocator allocator) {
    return {allocator, {}, log::ignore(), log::LogLevel::Off};
}

}
}
