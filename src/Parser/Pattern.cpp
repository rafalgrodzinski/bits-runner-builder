#include "Pattern.h"

#include "Parser/ValueType/ValueType.h"

using namespace std;

Pattern::Pattern(shared_ptr<ValueType> valueType, shared_ptr<Location> location):
location(location),
valueType(valueType) { }

std::shared_ptr<Location> Pattern::getLocation() const {
    return location;
}

shared_ptr<ValueType> Pattern::getValueType() const {
    return valueType;
}