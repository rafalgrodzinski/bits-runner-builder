#include "AnalyzerScopeBoxed.h"
#include "AnalyzerScope.h"

AnalyzerScopeBoxed::AnalyzerScopeBoxed(AnalyzerScope *parent):
parent(parent) { }

shared_ptr<ValueType> AnalyzerScopeBoxed::getNamedValueType(const string &namedValueTypeKey) const {
    stack<AnalyzerScope::ScopeLevel> scopeLevels = parent->scopeLevels;

    while (!scopeLevels.empty()) {
        auto it = scopeLevels.top().scopeLevelBoxed.namedValueTypesMap.find(namedValueTypeKey);
        if (it != scopeLevels.top().scopeLevelBoxed.namedValueTypesMap.end())
            return scopeLevels.top().scopeLevelBoxed.namedValueTypesMap[namedValueTypeKey];
        scopeLevels.pop();
    }

    return nullptr;
}

void AnalyzerScopeBoxed::registerNamedValueTypesMap(const vector<string> &namedValueTypeKeys, const vector<shared_ptr<ValueType>> &namedValueTypes) {
    for (int i=0; i<namedValueTypeKeys.size(); i++) {
        parent->scopeLevels.top().scopeLevelBoxed.namedValueTypesMap[namedValueTypeKeys[i]] = namedValueTypes[i];
    }

    //return AnalyzerScopeRegisterResult::SUCCES;
}