#pragma once

#include <functional>
#include <utility>
#include "token_concat.hpp"

#define CZ_DEFER(code) CZ_DEFER_FUNCTION([&]() { code; })
#define CZ_DEFER_FUNCTION(function) cz::Deferer CZ_TOKEN_CONCAT(defer_, __LINE__)(function);

namespace cz {

class Deferer {
    std::function<void()> function;

public:
    Deferer(std::function<void()> function) : function(std::move(function)) {}
    Deferer(const Deferer&) = delete;
    Deferer& operator=(const Deferer&) = delete;
    ~Deferer() { function(); }
};

}
