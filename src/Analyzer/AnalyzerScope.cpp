#include "AnalyzerScope.h"

AnalyzerScope::AnalyzerScope() {
    pushLevel();
}

void AnalyzerScope::pushLevel() {
    scopeLevels.push(ScopeLevel());
}

void AnalyzerScope::popLevel() {
    scopeLevels.pop();
}

shared_ptr<ValueType> AnalyzerScope::getVariableType(string identifier) {
    stack<ScopeLevel> scopeLevels = this->scopeLevels;

    while (!scopeLevels.empty()) {
        shared_ptr<ValueType> type = scopeLevels.top().variableTypes[identifier];
        if (type != nullptr)
            return type;
        scopeLevels.pop();
    }

    return nullptr;
}

bool AnalyzerScope::setVariableType(string identifier, shared_ptr<ValueType> type) {
    if (scopeLevels.top().variableTypes[identifier] != nullptr)
        return false;

    scopeLevels.top().variableTypes[identifier] = type;
    return true;
}

shared_ptr<ValueType> AnalyzerScope::getFunctionType(string name) {
    stack<ScopeLevel> scopeLevels = this->scopeLevels;

    while (!scopeLevels.empty()) {
        shared_ptr<ValueType> type = scopeLevels.top().functionTypeMap[name];
        if (type != nullptr)
            return type;
        scopeLevels.pop();
    }

    return nullptr;
}

bool AnalyzerScope::setFunctionType(string name, shared_ptr<ValueType> type) {
    if (scopeLevels.top().functionTypeMap[name] != nullptr)
        return false;

    scopeLevels.top().functionTypeMap[name] = type;
    return true;
}