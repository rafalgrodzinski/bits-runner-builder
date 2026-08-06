#include "AnalyzerScopeBlob.h"

#include "AnalyzerScope.h"

AnalyzerScopeBlob::AnalyzerScopeBlob(AnalyzerScope *parent):
parent(parent) { }

//optional<vector<string>> AnalyzerScopeBlob::getNamedValueTypeKeys(shared_ptr<SymbolName> symbolName) {
optional<vector<string>> AnalyzerScopeBlob::getNamedValueTypeKeys(string symbolName) {
    stack<AnalyzerScope::ScopeLevel> scopeLevels = parent->scopeLevels;

    while (!scopeLevels.empty()) {
        auto it = scopeLevels.top().blobNamedValueTypeKeysMap.find(symbolName);
        if (it != scopeLevels.top().blobNamedValueTypeKeysMap.end())
            return scopeLevels.top().blobNamedValueTypeKeysMap[symbolName];
        scopeLevels.pop();
    }

    return {};
}

//void AnalyzerScopeBlob::registerNamedValueTypeKeys(shared_ptr<SymbolName> symbolName, const vector<string> &namedValueTypeKeys) {
void AnalyzerScopeBlob::registerNamedValueTypeKeys(string symbolName, const vector<string> &namedValueTypeKeys) {
    // check if named types are already defined
    //if (scopeLevels.top().blobNamedTypeKeysMap.find(blobName) != scopeLevels.top().blobNamedTypeKeysMap.end())
    //    return false;

    parent->scopeLevels.top().blobNamedValueTypeKeysMap[symbolName] = namedValueTypeKeys;

    //return true;
}

optional<vector<shared_ptr<ValueType>>> AnalyzerScopeBlob::getFieldVariableValueTypes(string symbolName) {
    
}