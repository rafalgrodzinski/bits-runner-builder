#ifndef VALUE_TYPE_BOXED_H
#define VALUE_TYPE_BOXED_H

#include <optional>
#include "ValueType.h"

class ValueTypeBoxed: public ValueType {
friend class Analyzer;

public:
    ValueTypeBoxed(
        const optional<string> &namedValueTypeKey,
        shared_ptr<ValueType> boxedValueType,
        shared_ptr<Location> location
    );

    optional<string> getNamedValueTypeKey() const;
    shared_ptr<ValueType> getBoxedValueType() const;

    void setModuleName(const string &moduleName) override;
    bool isEqual(shared_ptr<ValueType> other) const override;
    shared_ptr<ValueType> clone() const override;

private:
    optional<string> namedValueTypeKey;
    shared_ptr<ValueType> boxedValueType;
};

#endif