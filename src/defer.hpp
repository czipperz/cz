#pragma once

#include <utility>
#include <functional>

#define CZ_DEFER(code) CZ_DEFER_FUNCTION([&]() { code; })
#define CZ_DEFER_FUNCTION(function) cz::Deferer defer_##__LINE__(function);

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
