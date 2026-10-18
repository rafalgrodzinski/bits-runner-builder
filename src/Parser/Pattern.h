#ifndef PATTERN_H
#define PATTERN_H

#include <memory>

class Location;
class ValueType;

class Pattern {
public:
    Pattern(std::shared_ptr<ValueType> valueType, std::shared_ptr<Location> location);

    std::shared_ptr<Location> getLocation() const;
    std::shared_ptr<ValueType> getValueType() const;

private:
    std::shared_ptr<Location> location;
    std::shared_ptr<ValueType> valueType;
};

#endif