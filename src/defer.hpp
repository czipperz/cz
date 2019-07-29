#pragma once

#define defer(code) defer_function([&]() { code })
#define defer_function(function) cz::Deferer defer_##__LINE__(code);

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
