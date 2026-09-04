#include "Defer.h"

Defer::Defer(const std::function<void ()> &fun) noexcept:
fun(fun) { }

Defer::~Defer() {
    if (fun)
        fun();
}