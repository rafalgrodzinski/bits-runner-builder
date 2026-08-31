#ifndef VALUE_TYPE_FUN_H
#define VALUE_TYPE_FUN_H

#include "ValueType.h"

class SymbolName;

class ValueTypeFun: public ValueType {
friend class Analyzer;

public:
    ValueTypeFun(
        const vector<shared_ptr<ValueType>> &argumentValueTypes,
        shared_ptr<ValueType> returnValueType
    );

    vector<shared_ptr<ValueType>> getArgumentValueTypes();
    shared_ptr<ValueType> getReturnValueType();

    bool isEqual(shared_ptr<ValueType> other) const override;

private:
    shared_ptr<SymbolName> symbolName;
    vector<shared_ptr<ValueType>> argumentValueTypes;
    shared_ptr<ValueType> returnValueType;
};

#endif