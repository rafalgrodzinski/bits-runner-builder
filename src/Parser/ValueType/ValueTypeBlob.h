#ifndef VALUE_TYPE_BLOB_H
#define VALUE_TYPE_BLOB_H

#include <optional>
#include <vector>
#include "ValueType.h"

class SymbolName;

class ValueTypeBlob: public ValueType {
friend class Analyzer;

public:
    ValueTypeBlob(
        const string &name,
        const vector<shared_ptr<ValueType>> &namedValueTypes,
        bool isPacked,
        shared_ptr<Location> location
    );

    shared_ptr<SymbolName> getSymbolName() const;
    shared_ptr<SymbolName> getPackedSymbolName() const;
    optional<vector<string>> getNamedValueTypeKeys() const;
    vector<shared_ptr<ValueType>> getNamedValueTypes() const;
    bool getIsPacked() const;

    void setModuleName(const string &moduleName) override;
    bool isEqual(shared_ptr<ValueType> other) const override;
    shared_ptr<ValueType> clone() const override;

private:
    shared_ptr<SymbolName> symbolName;
    optional<vector<string>> namedValueTypeKeys;
    vector<shared_ptr<ValueType>> namedValueTypes;
    bool isPacked;
};

#endif