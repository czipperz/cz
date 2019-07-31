#include <stddef.h>

namespace cz {

template <class T>
struct Slice {
    T* buffer;
    size_t len;
};

}
