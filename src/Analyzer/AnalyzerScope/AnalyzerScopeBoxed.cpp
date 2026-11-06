#include "AnalyzerScopeBoxed.h"

#include "AnalyzerScope.h"

using namespace std;

// MARK: - Public

AnalyzerScopeBoxed::AnalyzerScopeBoxed(AnalyzerScope *parent):
parent(parent) { }

shared_ptr<ValueType> AnalyzerScopeBoxed::getNamedValueType(const string &namedValueTypeKey) const {
    vector<AnalyzerScope::ScopeLevel> &scopeLevels = parent->scopeLevels;

    for (auto sit = scopeLevels.rbegin(); sit != scopeLevels.rend(); sit++) {
        auto it = (*sit).scopeLevelBoxed.namedValueTypesMap.find(namedValueTypeKey);
        if (it != (*sit).scopeLevelBoxed.namedValueTypesMap.end())
            return (*sit).scopeLevelBoxed.namedValueTypesMap[namedValueTypeKey];
    }

    return nullptr;
}

void AnalyzerScopeBoxed::registerNamedValueTypesMap(const vector<string> &namedValueTypeKeys, const vector<shared_ptr<ValueType>> &namedValueTypes) {
    if (namedValueTypes.empty())
        return;

    for (int i=0; i<namedValueTypeKeys.size(); i++) {
        parent->scopeLevels.back().scopeLevelBoxed.namedValueTypesMap[namedValueTypeKeys[i]] = namedValueTypes[i];
    }

    //return AnalyzerScopeRegisterResult::SUCCES;
}