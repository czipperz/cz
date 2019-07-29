#include "sized.hpp"

namespace cz {

const Sized sized_int = {sizeof(int), alignof(int)};

template <>
const Sized* sized<int>() {
    return &sized_int;
}

}
