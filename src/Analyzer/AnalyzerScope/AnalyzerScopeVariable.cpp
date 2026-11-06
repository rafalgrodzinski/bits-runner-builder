#include "AnalyzerScopeVariable.h"

#include "AnalyzerScope.h"
#include "Parser/ValueType/ValueType.h"

using namespace std;

// MARK: - Public

AnalyzerScopeVariable::AnalyzerScopeVariable(AnalyzerScope *parent):
parent(parent) { }

std::shared_ptr<ValueType> AnalyzerScopeVariable::getValueType(const std::string &identifier) const {
    vector<AnalyzerScope::ScopeLevel> &scopeLevels = parent->scopeLevels;

    for (auto sit = scopeLevels.rbegin(); sit != scopeLevels.rend(); sit++) {
        auto it = (*sit).scopeLevelVariable.valueTypesMap.find(identifier);        
        if (it != (*sit).scopeLevelVariable.valueTypesMap.end())
            return (*sit).scopeLevelVariable.valueTypesMap[identifier];
    }

    return nullptr;
}

AnalyzerScopeRegisterResult AnalyzerScopeVariable::registerValueType(const std::string &identifier, std::shared_ptr<ValueType> valueType, bool isDefinition) {
    AnalyzerScope::ScopeLevel &topScopeLevels = parent->scopeLevels.back();

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
    AnalyzerScopeRegisterResult result = registerValueType(
        format("${}", parent->scopeLevels.back().scopeLevelVariable.anonymousValueTypesCount),
        valueType,
        true
    );

    if (result == AnalyzerScopeRegisterResult::SUCCES)
        parent->scopeLevels.back().scopeLevelVariable.anonymousValueTypesCount++;
    
    return result;
}