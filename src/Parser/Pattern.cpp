#include "Pattern.h"

#include "Parser/ValueType/ValueType.h"

using namespace std;

Pattern::Pattern(shared_ptr<ValueType> valueType):
valueType(valueType) { }

shared_ptr<ValueType> Pattern::getValueType() {
    return valueType;
}