#ifndef VALUE_TYPE_BLOB_H
#define VALUE_TYPE_BLOB_H

#include "ValueType.h"
#include "Parser/SymbolName.h"

class ValueTypeBlob: public ValueType {
friend class Analyzer;

public:
    ValueTypeBlob(const string &name, const vector<shared_ptr<ValueType>> &namedValueTypes);

    shared_ptr<SymbolName> getSymbolName() const;
    void setModuleName(const string &moduleName) override;
    bool isEqual(shared_ptr<ValueType> other) const override;
    optional<vector<string>> getNamedValueTypeKeys();
    vector<shared_ptr<ValueType>> getNamedValueTypes();

private:
    shared_ptr<SymbolName> symbolName;
    optional<vector<string>> namedValueTypeKeys;
    vector<shared_ptr<ValueType>> namedValueTypes;
};

#endif