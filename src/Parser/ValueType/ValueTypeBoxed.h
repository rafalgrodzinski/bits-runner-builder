#ifndef VALUE_TYPE_BOXED_H
#define VALUE_TYPE_BOXED_H

#include "ValueType.h"

class ValueTypeBoxed: public ValueType {
friend class Analyzer;

public:
    ValueTypeBoxed(const optional<string> &namedValueTypeKey, shared_ptr<ValueType> boxedValueType);

    optional<string> getNamedValueTypeKey();
    shared_ptr<ValueType> getBoxedValueType();
    virtual bool isEqual(shared_ptr<ValueType> other) const override;
    virtual shared_ptr<ValueType> clone() const override;

private:
    optional<string> namedValueTypeKey;
    shared_ptr<ValueType> boxedValueType;
};

#endif