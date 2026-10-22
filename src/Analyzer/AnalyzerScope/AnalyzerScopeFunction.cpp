#include "AnalyzerScopeFunction.h"

#include "AnalyzerScope.h"
#include "Parser/ValueType/ValueType.h"

using namespace std;

// MARK: - Public

AnalyzerScopeFunction::AnalyzerScopeFunction(AnalyzerScope *parent):
parent(parent) { }

shared_ptr<ValueType> AnalyzerScopeFunction::getValueType(const string &name) const {
    stack<AnalyzerScope::ScopeLevel> scopeLevels = parent->scopeLevels;

    while (!scopeLevels.empty()) {
        auto it = scopeLevels.top().scopeLevelFunction.valueTypesMap.find(name);        
        if (it != scopeLevels.top().scopeLevelFunction.valueTypesMap.end())
            return scopeLevels.top().scopeLevelFunction.valueTypesMap[name];
        scopeLevels.pop();
    }

    return nullptr;
}

AnalyzerScopeRegisterResult AnalyzerScopeFunction::registerValueType(const string &name, shared_ptr<ValueType> valueType, bool isDefinition) {
    AnalyzerScope::ScopeLevel &topScopeLevels = parent->scopeLevels.top();

    // Figure out existing registration state
    AnalyzerScopeState state = AnalyzerScopeState::NOT_REGISTERED;
    auto it = topScopeLevels.scopeLevelFunction.registerStatesMap.find(name);
    if (it != topScopeLevels.scopeLevelFunction.registerStatesMap.end())
        state = topScopeLevels.scopeLevelFunction.registerStatesMap[name];

    // Cannot define multiple times
    if (state == AnalyzerScopeState::DEFINED && isDefinition) {
        return AnalyzerScopeRegisterResult::FAILURE_ALREAD_DEFINED;
    // Declared type has to match
    } else if (state != AnalyzerScopeState::NOT_REGISTERED && !isDefinition) {
        shared_ptr<ValueType> registeredValueType = topScopeLevels.scopeLevelFunction.valueTypesMap[name];
        if (registeredValueType->isEqual(valueType)) {
            return AnalyzerScopeRegisterResult::SUCCES;
        } else {
            return AnalyzerScopeRegisterResult::FAILURE_ALREADY_DECLARED;
        }
    }

    // Register new value type
    topScopeLevels.scopeLevelFunction.valueTypesMap[name] = valueType;
    if (isDefinition) {
        topScopeLevels.scopeLevelFunction.registerStatesMap[name] = AnalyzerScopeState::DEFINED;
    } else {
        topScopeLevels.scopeLevelFunction.registerStatesMap[name] = AnalyzerScopeState::DECLARED;
    }

    return AnalyzerScopeRegisterResult::SUCCES;
}