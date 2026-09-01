#ifndef VALUE_TYPE_ENUM_H
#define VALUE_TYPE_ENUM_H

#include <map>

#include "ValueType.h"
#include "Parser/SymbolName.h"

class ValueTypeEnum: public ValueType {
friend class Analyzer;

public:
    ValueTypeEnum(const string &name, const vector<shared_ptr<ValueType>> &namedValueTypes);

    shared_ptr<SymbolName> getSymbolName() const;
    vector<shared_ptr<ValueType>> getNamedValueTypes();

    void setModuleName(const string &moduleName) override;
    bool isEqual(shared_ptr<ValueType> other) const override;
    shared_ptr<ValueType> clone() const override;

private:
    shared_ptr<SymbolName> symbolName;
    optional<vector<string>> namedValueTypeKeys;
    vector<shared_ptr<ValueType>> namedValueTypes;
};

#endif