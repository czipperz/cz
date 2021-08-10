#pragma once

#include "string.hpp"

namespace cz {
namespace env {

bool get(const char* key, Allocator allocator, String* value);
bool set(const char* key, const char* value);
bool remove(const char* key);

bool get_home(Allocator allocator, String* value);

}
}
