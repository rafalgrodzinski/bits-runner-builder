#include "AnalyzerScopeBlob.h"

#include "AnalyzerScope.h"
#include "Parser/ValueType/ValueType.h"

using namespace std;

// MARK: - Public

AnalyzerScopeBlob::AnalyzerScopeBlob(AnalyzerScope *parent):
parent(parent) { }

void AnalyzerScopeBlob::registerDeclaration(shared_ptr<SymbolName> symbolName) {
    if (getState(symbolName) != AnalyzerScopeState::NOT_REGISTERED)
        return;

    parent->scopeLevels.back().scopeLevelBlob.statesMap[*symbolName] = AnalyzerScopeState::DECLARED;
}

AnalyzerScopeState AnalyzerScopeBlob::getState(shared_ptr<SymbolName> symbolName) {
    vector<AnalyzerScope::ScopeLevel> &scopeLevels = parent->scopeLevels;

    for (auto sit = scopeLevels.rbegin(); sit != scopeLevels.rend(); sit++) {
        auto it = (*sit).scopeLevelBlob.statesMap.find(*symbolName);
        if (it != (*sit).scopeLevelBlob.statesMap.end())
            return (*sit).scopeLevelBlob.statesMap[*symbolName];
    }

    return AnalyzerScopeState::NOT_REGISTERED;
}

void AnalyzerScopeBlob::registerNamedValueTypeKeys(shared_ptr<SymbolName> symbolName, const vector<string> &namedValueTypeKeys) {
    // check if named types are already defined
    //if (scopeLevels.top().blobNamedTypeKeysMap.find(blobName) != scopeLevels.top().blobNamedTypeKeysMap.end())
    //    return false;

    parent->scopeLevels.back().scopeLevelBlob.namedValueTypeKeysMap[*symbolName] = namedValueTypeKeys;

    //return true;
}

optional<vector<string>> AnalyzerScopeBlob::getNamedValueTypeKeys(shared_ptr<SymbolName> symbolName) const {
    vector<AnalyzerScope::ScopeLevel> &scopeLevels = parent->scopeLevels;

    for (auto sit = scopeLevels.rbegin(); sit != scopeLevels.rend(); sit++) {
        auto it = (*sit).scopeLevelBlob.namedValueTypeKeysMap.find(*symbolName);
        if (it != (*sit).scopeLevelBlob.namedValueTypeKeysMap.end())
            return (*sit).scopeLevelBlob.namedValueTypeKeysMap[*symbolName];
    }

    return {};
}

void AnalyzerScopeBlob::registerFields(shared_ptr<SymbolName> symbolName, vector<pair<string, shared_ptr<ValueType>>> &fields) {
    parent->scopeLevels.back().scopeLevelBlob.fieldsMap[*symbolName] = fields;
}

optional<vector<pair<string, shared_ptr<ValueType>>>> AnalyzerScopeBlob::getFields(shared_ptr<SymbolName> symbolName) const {
    vector<AnalyzerScope::ScopeLevel> &scopeLevels = parent->scopeLevels;

    for (auto sit = scopeLevels.rbegin(); sit != scopeLevels.rend(); sit++) {
        auto it = (*sit).scopeLevelBlob.fieldsMap.find(*symbolName);
        if (it != (*sit).scopeLevelBlob.fieldsMap.end()) {
            vector<pair<string, shared_ptr<ValueType>>> fields = *it->second;

            vector<pair<string, shared_ptr<ValueType>>> clonedFields;
            for (pair<string, shared_ptr<ValueType>> &field : fields) {
                clonedFields.push_back({field.first, field.second->clone()});
            }
            return clonedFields;
        }
    }

    return {};
}

optional<vector<shared_ptr<ValueType>>> AnalyzerScopeBlob::getVariableFieldValueTypes(shared_ptr<SymbolName> symbolName) const {
    optional<vector<pair<string, shared_ptr<ValueType>>>> fields = getFields(symbolName);
        if (!fields)
            return { };

    vector<shared_ptr<ValueType>> valueTypes;
    for (pair<string, shared_ptr<ValueType>> &field : *fields) {
        if (!field.second->isFun())
            valueTypes.push_back(field.second);
    }

    return valueTypes;
}

void AnalyzerScopeBlob::registerConformingProtoSymbolNames(shared_ptr<SymbolName> symbolName, vector<shared_ptr<SymbolName>> conformingProtoSymbolNames) {
    parent->scopeLevels.back().scopeLevelBlob.conformingProtoSymbolNamsMap[*symbolName] = conformingProtoSymbolNames;
}

optional<vector<shared_ptr<SymbolName>>> AnalyzerScopeBlob::getConformingProtoSymbolNames(shared_ptr<SymbolName> symbolName) {
    vector<AnalyzerScope::ScopeLevel> &scopeLevels = parent->scopeLevels;

    for (auto sit = scopeLevels.rbegin(); sit != scopeLevels.rend(); sit++) {
        auto it = (*sit).scopeLevelBlob.conformingProtoSymbolNamsMap.find(*symbolName);
        if (it != (*sit).scopeLevelBlob.conformingProtoSymbolNamsMap.end())
            return (*sit).scopeLevelBlob.conformingProtoSymbolNamsMap[*symbolName];
    }

    return {};
}