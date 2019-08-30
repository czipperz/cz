#pragma once

#define CATCH_CONFIG_ENABLE_BENCHMARKING

#define CATCH_CONFIG_FALLBACK_STRINGIFIER(value) (cz::sprint_std(cz::format::debug(value)))
#include <cz/format.hpp>
#include <cz/std_string_writer.hpp>

#include "../catch.hpp"
