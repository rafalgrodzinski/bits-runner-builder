#ifndef PATTERN_H
#define PATTERN_H

#include <memory>

class ValueType;

class Pattern {
public:
    Pattern(std::shared_ptr<ValueType> valueType);

    std::shared_ptr<ValueType> getValueType();

private:
    std::shared_ptr<ValueType> valueType;
};

#endif