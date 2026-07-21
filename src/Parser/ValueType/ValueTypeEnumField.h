#ifndef VALUE_TYPE_ENUM_FIELD_H
#define VALUE_TYPE_ENUM_FIELD_H

#include "ValueType.h"
#include "Parser/SymbolName.h"

class ValueTypeEnumField: public ValueType {
public:
    ValueTypeEnumField(const string &name, const vector<shared_ptr<ValueType>> namedTypeValues);
    shared_ptr<SymbolName> getSymbolName() const;
    void setModuleName(const string &moduleName) override;
    bool isEqual(shared_ptr<ValueType> other) const override;
    bool canImplicitCastTo(shared_ptr<ValueType> other) const override;

private:
    shared_ptr<SymbolName> symbolName;
};

#endif