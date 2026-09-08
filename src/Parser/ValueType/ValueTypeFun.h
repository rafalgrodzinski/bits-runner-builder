#ifndef VALUE_TYPE_FUN_H
#define VALUE_TYPE_FUN_H

#include <vector>
#include "ValueType.h"

class SymbolName;

class ValueTypeFun: public ValueType {
friend class Analyzer;

public:
    ValueTypeFun(
        const vector<shared_ptr<ValueType>> &argumentValueTypes,
        shared_ptr<ValueType> returnValueType,
        shared_ptr<Location> location
    );

    vector<shared_ptr<ValueType>> getArgumentValueTypes() const;
    shared_ptr<ValueType> getReturnValueType() const;

    void setModuleName(const string &moduleName) override;
    bool isEqual(shared_ptr<ValueType> other) const override;
    shared_ptr<ValueType> clone() const override;

private:
    shared_ptr<SymbolName> symbolName;
    vector<shared_ptr<ValueType>> argumentValueTypes;
    shared_ptr<ValueType> returnValueType;
};

#endif