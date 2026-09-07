#include "AnalyzerScopeEnum.h"
#include "AnalyzerScope.h"

AnalyzerScopeEnum::AnalyzerScopeEnum(AnalyzerScope *parent):
parent(parent) { }

optional<vector<string>> AnalyzerScopeEnum::getNamedValueTypeKeys(shared_ptr<SymbolName> symbolName) {
    stack<AnalyzerScope::ScopeLevel> scopeLevels = parent->scopeLevels;

    while (!scopeLevels.empty()) {
        auto it = scopeLevels.top().scopeLevelEnum.namedValueTypeKeys.find(*symbolName);
        if (it != scopeLevels.top().scopeLevelEnum.namedValueTypeKeys.end())
            return scopeLevels.top().scopeLevelEnum.namedValueTypeKeys[*symbolName];
        scopeLevels.pop();
    }

    return {};
}

void AnalyzerScopeEnum::registerNamedValueTypeKeys(shared_ptr<SymbolName> symbolName, const vector<string> &namedValueTypeKeys) {
    optional<vector<string>> &oDeclaredNamedValueTypeKeys = parent->scopeLevels.top().scopeLevelEnum.namedValueTypeKeys[*symbolName];

    // If it's already declared, check
    if (oDeclaredNamedValueTypeKeys) {
        vector<string> &declaredNamedValueTypeKeys = *oDeclaredNamedValueTypeKeys;
        for (const string &namedValueTypeKey : namedValueTypeKeys) {
            // first check if each of the named types is not yet declared
            for (string &declaredNamedValueTypeKey : declaredNamedValueTypeKeys) {
                if (declaredNamedValueTypeKey == namedValueTypeKey)
                    //return AnalyzerScopeRegisterResult::FAILURE_ALREADY_DECLARED;
                    return;
            }
        }
    } else {
        oDeclaredNamedValueTypeKeys = vector<string>();
    }

    for (const string &namedValueTypeKey : namedValueTypeKeys) {
        (*oDeclaredNamedValueTypeKeys).push_back(namedValueTypeKey);
    }

    //return AnalyzerScopeRegisterResult::SUCCES;
}

shared_ptr<ValueType> AnalyzerScopeEnum::getPayloadValueType(shared_ptr<SymbolName> symbolName) {
    stack<AnalyzerScope::ScopeLevel> scopeLevels = parent->scopeLevels;

    while (!scopeLevels.empty()) {
        auto it = scopeLevels.top().scopeLevelEnum.payloadValueType.find(*symbolName);
        if (it != scopeLevels.top().scopeLevelEnum.payloadValueType.end())
            return scopeLevels.top().scopeLevelEnum.payloadValueType[*symbolName];
        scopeLevels.pop();
    }

    return nullptr;
}

void AnalyzerScopeEnum::registerPayloadValueType(shared_ptr<SymbolName> symbolName, shared_ptr<ValueType> payloadValueType) {
    parent->scopeLevels.top().scopeLevelEnum.payloadValueType[*symbolName] = payloadValueType;
    //return AnalyzerScopeRegisterResult::SUCCES;
}