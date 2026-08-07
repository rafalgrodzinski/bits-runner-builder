#ifndef ANALYZER_SCOPE_BLOB_H
#define ANALYZER_SCOPE_BLOB_H

#include <map>
#include <memory>
#include <optional>
#include <stack>
#include <string>
#include <vector>
#include "Parser/SymbolName.h"

class AnalyzerScope;
class ValueType;

using namespace std;

class AnalyzerScopeBlob {
public:
    typedef struct {
        map<SymbolName, optional<vector<string>>> namedValueTypeKeysMap;
        map<SymbolName, optional<vector<pair<string, shared_ptr<ValueType>>>>> fieldsMap;
    } ScopeLevel;

    AnalyzerScopeBlob(AnalyzerScope *parent);

    void registerNamedValueTypeKeys(shared_ptr<SymbolName> symbolName, const vector<string> &namedValueTypeKeys);
    optional<vector<string>> getNamedValueTypeKeys(shared_ptr<SymbolName> symbolName) const;

    void registerFields(shared_ptr<SymbolName> symbolName, vector<pair<string, shared_ptr<ValueType>>> &fields);
    optional<vector<pair<string, shared_ptr<ValueType>>>> getFields(shared_ptr<SymbolName> symbolName) const;
    optional<vector<shared_ptr<ValueType>>> getVariableFieldValueTypes(shared_ptr<SymbolName> symbolName) const;

private:
    AnalyzerScope *parent;
};

#endif