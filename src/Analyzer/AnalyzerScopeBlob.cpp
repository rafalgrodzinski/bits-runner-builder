#include "AnalyzerScopeBlob.h"
#include "AnalyzerScope.h"

AnalyzerScopeBlob::AnalyzerScopeBlob(AnalyzerScope *parent):
parent(parent) { }

optional<vector<string>> AnalyzerScopeBlob::getNamedValueTypeKeys(shared_ptr<SymbolName> symbolName) {
    stack<AnalyzerScope::ScopeLevel> scopeLevels = parent->scopeLevels;

    while (!scopeLevels.empty()) {
        auto it = scopeLevels.top().scopeLevelBlob.namedValueTypeKeysMap.find(*symbolName);
        if (it != scopeLevels.top().scopeLevelBlob.namedValueTypeKeysMap.end())
            return scopeLevels.top().scopeLevelBlob.namedValueTypeKeysMap[*symbolName];
        scopeLevels.pop();
    }

    return {};
}

void AnalyzerScopeBlob::registerNamedValueTypeKeys(shared_ptr<SymbolName> symbolName, const vector<string> &namedValueTypeKeys) {
    // check if named types are already defined
    //if (scopeLevels.top().blobNamedTypeKeysMap.find(blobName) != scopeLevels.top().blobNamedTypeKeysMap.end())
    //    return false;

    parent->scopeLevels.top().scopeLevelBlob.namedValueTypeKeysMap[*symbolName] = namedValueTypeKeys;

    //return true;
}

optional<vector<shared_ptr<ValueType>>> AnalyzerScopeBlob::getFieldVariableValueTypes(shared_ptr<SymbolName> symbolName) {
    
}