#ifndef VALUE_TYPE_ENUM_FIELD_H
#define VALUE_TYPE_ENUM_FIELD_H

#include <map>

#include "ValueType.h"
#include "Parser/SymbolName.h"

class ValueTypeEnumField: public ValueType {
friend class Analyzer;

public:
    ValueTypeEnumField(const string &name, const vector<shared_ptr<ValueType>> namedValueTypes);

    shared_ptr<SymbolName> getSymbolName() const;
    vector<shared_ptr<ValueType>> getNamedValueTypes() const;
    shared_ptr<ValueType> getPayloadValueType() const;

    void setModuleName(const string &moduleName) override;
    bool isEqual(shared_ptr<ValueType> other) const override;
    shared_ptr<ValueType> clone() const override;
    
private:
    shared_ptr<SymbolName> symbolName;
    vector<string> namedValueTypeKeys;
    vector<shared_ptr<ValueType>> namedValueTypes;
    shared_ptr<ValueType> payloadValueType;
};

#endif