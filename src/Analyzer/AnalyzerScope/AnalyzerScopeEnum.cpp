#include "AnalyzerScopeEnum.h"

#include "AnalyzerScope.h"
#include "Parser/SymbolName.h"

using namespace std;

// MARK: - Public

AnalyzerScopeEnum::AnalyzerScopeEnum(AnalyzerScope *parent):
parent(parent) { }

optional<vector<string>> AnalyzerScopeEnum::getNamedValueTypeKeys(shared_ptr<SymbolName> symbolName) {
    stack<AnalyzerScope::ScopeLevel> scopeLevels = parent->scopeLevels;

    while (!scopeLevels.empty()) {
        auto it = scopeLevels.top().scopeLevelEnum.namedValueTypeKeysMap.find(*symbolName);
        if (it != scopeLevels.top().scopeLevelEnum.namedValueTypeKeysMap.end())
            return scopeLevels.top().scopeLevelEnum.namedValueTypeKeysMap[*symbolName];
        scopeLevels.pop();
    }

    return {};
}

AnalyzerScopeRegisterResult AnalyzerScopeEnum::registerNamedValueTypeKeys(shared_ptr<SymbolName> symbolName, const vector<string> &namedValueTypeKeys) {
    optional<vector<string>> &oDeclaredNamedValueTypeKeys = parent->scopeLevels.top().scopeLevelEnum.namedValueTypeKeysMap[*symbolName];

    // If it's already declared, check
    if (oDeclaredNamedValueTypeKeys) {
        vector<string> &declaredNamedValueTypeKeys = *oDeclaredNamedValueTypeKeys;
        for (const string &namedValueTypeKey : namedValueTypeKeys) {
            // first check if each of the named types is not yet declared
            for (string &declaredNamedValueTypeKey : declaredNamedValueTypeKeys) {
                if (declaredNamedValueTypeKey == namedValueTypeKey)
                    return AnalyzerScopeRegisterResult::FAILURE_ALREADY_DECLARED;
            }
        }
    } else {
        oDeclaredNamedValueTypeKeys = vector<string>();
    }

    for (const string &namedValueTypeKey : namedValueTypeKeys) {
        (*oDeclaredNamedValueTypeKeys).push_back(namedValueTypeKey);
    }

    return AnalyzerScopeRegisterResult::SUCCES;
}

shared_ptr<ValueType> AnalyzerScopeEnum::getPayloadValueType(shared_ptr<SymbolName> symbolName) {
    stack<AnalyzerScope::ScopeLevel> scopeLevels = parent->scopeLevels;

    while (!scopeLevels.empty()) {
        auto it = scopeLevels.top().scopeLevelEnum.payloadValueTypesMap.find(*symbolName);
        if (it != scopeLevels.top().scopeLevelEnum.payloadValueTypesMap.end())
            return scopeLevels.top().scopeLevelEnum.payloadValueTypesMap[*symbolName];
        scopeLevels.pop();
    }

    return nullptr;
}

AnalyzerScopeRegisterResult AnalyzerScopeEnum::registerPayloadValueType(shared_ptr<SymbolName> symbolName, shared_ptr<ValueType> payloadValueType) {
    auto it = parent->scopeLevels.top().scopeLevelEnum.payloadValueTypesMap.find(*symbolName);
    if (it != parent->scopeLevels.top().scopeLevelEnum.payloadValueTypesMap.end())
        return AnalyzerScopeRegisterResult::FAILURE_ALREAD_DEFINED;

    parent->scopeLevels.top().scopeLevelEnum.payloadValueTypesMap[*symbolName] = payloadValueType;
    return AnalyzerScopeRegisterResult::SUCCES;
}