#ifndef DEFER_H
#define DEFER_H

#include <functional>

class Defer {
public:
    Defer(const std::function<void ()> &fun) noexcept;
    ~Defer();
    Defer(const Defer&) = delete;
    Defer &operator = (const Defer&) = delete;

private:
    std::function<void ()> fun;
};

#endif