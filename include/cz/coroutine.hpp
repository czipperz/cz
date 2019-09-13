#pragma once

namespace cz {

struct Coroutine {
    long _state = 0;
};

#define CZ_CO_START   \
    switch (_state) { \
        case 0:
#define CZ_CO_END }

#define CZ_CO_YIELD(result) \
    do {                    \
        _state = __LINE__;  \
        return result;      \
        case __LINE__:;     \
    } while (0)

}
