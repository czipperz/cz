#pragma once

#define CATCH_CONFIG_ENABLE_BENCHMARKING

#define CATCH_CONFIG_FALLBACK_STRINGIFIER(value) (cz::std_string_format(value))
#include <cz/format.hpp>
#include <cz/std_string_format.hpp>

#include "../catch.hpp"
