#ifndef VALUE_TYPE_ENUM_H
#define VALUE_TYPE_ENUM_H

#include <map>

#include "ValueType.h"
#include "Parser/SymbolName.h"

class ValueTypeEnum: public ValueType {
public:
    ValueTypeEnum(const string &name, const vector<shared_ptr<ValueType>> &namedValueTypes);

    shared_ptr<SymbolName> getSymbolName() const;
    void setModuleName(const string &moduleName) override;
    bool isEqual(shared_ptr<ValueType> other) const override;
    bool canImplicitCastTo(shared_ptr<ValueType> other) const override;
    //map<string, shared_ptr<ValueType>> getNamedTypesMap();
    vector<shared_ptr<ValueType>> getNamedValueTypes();
    void setNamedValueTypeKeys(const vector<string> &namedValueTypeKeys);

private:
    shared_ptr<SymbolName> symbolName;
    vector<shared_ptr<ValueType>> namedValueTypes;
    //map<string, shared_ptr<ValueType>> namedTypesMap;
    optional<vector<string>> namedValueTypeKeys;
};

#endif