#include "AnalyzerScopeVariable.h"

#include "AnalyzerScope.h"

using namespace std;

// MAKR: - Public

AnalyzerScopeVariable::AnalyzerScopeVariable(AnalyzerScope *parent):
parent(parent) { }

std::shared_ptr<ValueType> AnalyzerScopeVariable::getValueType(const std::string &name) const {
    stack<AnalyzerScope::ScopeLevel> scopeLevels = parent->scopeLevels;

    while (!scopeLevels.empty()) {
        auto it = scopeLevels.top().scopeLevelVariable.valueTypesMap.find(name);        
        if (it != scopeLevels.top().scopeLevelVariable.valueTypesMap.end())
            return scopeLevels.top().scopeLevelVariable.valueTypesMap[name];
        scopeLevels.pop();
    }

    return nullptr;
}

void AnalyzerScopeVariable::registerValueType(const std::string &name, std::shared_ptr<ValueType> valueType) {
    parent->scopeLevels.top().scopeLevelVariable.valueTypesMap[name] = valueType;
}

void AnalyzerScopeVariable::registerAnonymousValueType(std::shared_ptr<ValueType> valueType) {
    stack<AnalyzerScope::ScopeLevel> scopeLevels = parent->scopeLevels;

    registerValueType(format("${}", scopeLevels.top().scopeLevelVariable.AnonymousValueTypesCount), valueType);
    scopeLevels.top().scopeLevelVariable.AnonymousValueTypesCount++;
}