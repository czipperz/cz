#pragma once

#define CZ_DEFER(code) CZ_DEFER_FUNCTION([&]() { code })
#define CZ_DEFER_FUNCTION(function) cz::Deferer defer_##__LINE__(code);

namespace cz {

template <class F>
class Deferer {
    F function;

public:
    Deferer(F function) : function(function) {}
    Deferer(const Deferer&) = delete;
    Deferer& operator=(const Deferer&) = delete;
    ~Deferer() { function(); }
};

}
