#include "AnalyzerScopeBlob.h"
#include "AnalyzerScope.h"
#include "Parser/ValueType/ValueType.h"

AnalyzerScopeBlob::AnalyzerScopeBlob(AnalyzerScope *parent):
parent(parent) { }

void AnalyzerScopeBlob::registerNamedValueTypeKeys(shared_ptr<SymbolName> symbolName, const vector<string> &namedValueTypeKeys) {
    // check if named types are already defined
    //if (scopeLevels.top().blobNamedTypeKeysMap.find(blobName) != scopeLevels.top().blobNamedTypeKeysMap.end())
    //    return false;

    parent->scopeLevels.top().scopeLevelBlob.namedValueTypeKeysMap[*symbolName] = namedValueTypeKeys;

    //return true;
}

optional<vector<string>> AnalyzerScopeBlob::getNamedValueTypeKeys(shared_ptr<SymbolName> symbolName) const {
    stack<AnalyzerScope::ScopeLevel> scopeLevels = parent->scopeLevels;

    while (!scopeLevels.empty()) {
        auto it = scopeLevels.top().scopeLevelBlob.namedValueTypeKeysMap.find(*symbolName);
        if (it != scopeLevels.top().scopeLevelBlob.namedValueTypeKeysMap.end())
            return scopeLevels.top().scopeLevelBlob.namedValueTypeKeysMap[*symbolName];
        scopeLevels.pop();
    }

    return {};
}

void AnalyzerScopeBlob::registerFields(shared_ptr<SymbolName> symbolName, vector<pair<string, shared_ptr<ValueType>>> &fields) {
    parent->scopeLevels.top().scopeLevelBlob.fieldsMap[*symbolName] = fields;
}

optional<vector<pair<string, shared_ptr<ValueType>>>> AnalyzerScopeBlob::getFields(shared_ptr<SymbolName> symbolName) const {
    stack<AnalyzerScope::ScopeLevel> scopeLevels = parent->scopeLevels;

    while (!scopeLevels.empty()) {
        auto it = scopeLevels.top().scopeLevelBlob.fieldsMap.find(*symbolName);
        if (it != scopeLevels.top().scopeLevelBlob.fieldsMap.end())
            return scopeLevels.top().scopeLevelBlob.fieldsMap[*symbolName];
        scopeLevels.pop();
    }

    return {};
}

optional<vector<shared_ptr<ValueType>>> AnalyzerScopeBlob::getVariableFieldValueTypes(shared_ptr<SymbolName> symbolName) const {
    optional<vector<pair<string, shared_ptr<ValueType>>>> fields = getFields(symbolName);
        if (!fields)
            return { };

    vector<shared_ptr<ValueType>> valueTypes;
    for (pair<string, shared_ptr<ValueType>> &field : *fields) {
        if (!field.second->isFunction())
            valueTypes.push_back(field.second);
    }

    return valueTypes;
}