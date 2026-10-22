#include "AnalyzerScopeVariable.h"

#include "AnalyzerScope.h"
#include "Parser/ValueType/ValueType.h"

using namespace std;

// MAKR: - Public

AnalyzerScopeVariable::AnalyzerScopeVariable(AnalyzerScope *parent):
parent(parent) { }

std::shared_ptr<ValueType> AnalyzerScopeVariable::getValueType(const std::string &identifier) const {
    stack<AnalyzerScope::ScopeLevel> scopeLevels = parent->scopeLevels;

    while (!scopeLevels.empty()) {
        auto it = scopeLevels.top().scopeLevelVariable.valueTypesMap.find(identifier);        
        if (it != scopeLevels.top().scopeLevelVariable.valueTypesMap.end())
            return scopeLevels.top().scopeLevelVariable.valueTypesMap[identifier];
        scopeLevels.pop();
    }

    return nullptr;
}

AnalyzerScopeRegisterResult AnalyzerScopeVariable::registerValueType(const std::string &identifier, std::shared_ptr<ValueType> valueType, bool isDefinition) {
    AnalyzerScope::ScopeLevel &topScopeLevels = parent->scopeLevels.top();

    // Figure out existing registration state
    AnalyzerScopeState state = AnalyzerScopeState::NOT_REGISTERED;
    auto it = topScopeLevels.scopeLevelVariable.statesMap.find(identifier);
    if (it != topScopeLevels.scopeLevelVariable.statesMap.end())
        state = topScopeLevels.scopeLevelVariable.statesMap[identifier];

    // Cannot define multiple times
    if (state == AnalyzerScopeState::DEFINED && isDefinition) {
        return AnalyzerScopeRegisterResult::FAILURE_ALREAD_DEFINED;
    // Declared type has to match
    } else if (state != AnalyzerScopeState::NOT_REGISTERED && !isDefinition) {
        shared_ptr<ValueType> registeredValueType = topScopeLevels.scopeLevelVariable.valueTypesMap[identifier];
        if (registeredValueType->isEqual(valueType)) {
            return AnalyzerScopeRegisterResult::SUCCES;
        } else {
            return AnalyzerScopeRegisterResult::FAILURE_ALREADY_DECLARED;
        }
    }

    // Register new value type
    topScopeLevels.scopeLevelVariable.valueTypesMap[identifier] = valueType;
    if (isDefinition) {
        topScopeLevels.scopeLevelVariable.statesMap[identifier] = AnalyzerScopeState::DEFINED;
    } else {
        topScopeLevels.scopeLevelVariable.statesMap[identifier] = AnalyzerScopeState::DECLARED;
    }

    return AnalyzerScopeRegisterResult::SUCCES;
}

AnalyzerScopeRegisterResult AnalyzerScopeVariable::registerAnonymousValueType(std::shared_ptr<ValueType> valueType) {
    stack<AnalyzerScope::ScopeLevel> scopeLevels = parent->scopeLevels;

    AnalyzerScopeRegisterResult result = registerValueType(format("${}", scopeLevels.top().scopeLevelVariable.anonymousValueTypesCount), valueType, true);

    if (result == AnalyzerScopeRegisterResult::SUCCES)
        scopeLevels.top().scopeLevelVariable.anonymousValueTypesCount++;
    
    return result;
}