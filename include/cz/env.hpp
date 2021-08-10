#pragma once

#include "string.hpp"
#include "vector.hpp"

namespace cz {
namespace env {

bool get(const char* key, Allocator allocator, String* value);
bool set(const char* key, const char* value);
bool remove(const char* key);

bool get_home(Allocator allocator, String* value);

bool get_path(Allocator vector_allocator,
              Allocator value_allocator,
              Vector<Str>* values,
              String* total);
bool get_path_extensions(Allocator vector_allocator,
                         Allocator value_allocator,
                         Vector<Str>* values,
                         String* total);

bool in_path(Str file);

}
}
