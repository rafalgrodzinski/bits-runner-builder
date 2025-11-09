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

optional<vector<shared_ptr<ValueType>>> AnalyzerScope::getFunctionArgumentTypes(string name) {
    stack<ScopeLevel> scopeLevels = this->scopeLevels;

    while (!scopeLevels.empty()) {
        auto it = scopeLevels.top().functionArgumentTypesMap.find(name);
        if (it != scopeLevels.top().functionArgumentTypesMap.end())
            return it->second;
        scopeLevels.pop();
    }

    return { };
}

bool AnalyzerScope::setFunctionArgumentTypes(string name, vector<shared_ptr<ValueType>> argumentTypes) {
    auto it = scopeLevels.top().functionArgumentTypesMap.find(name);
    if (it != scopeLevels.top().functionArgumentTypesMap.end())
        return false;

    scopeLevels.top().functionArgumentTypesMap[name] = argumentTypes;
    return true;
}

shared_ptr<ValueType> AnalyzerScope::getFunctionReturnType(string name) {
    stack<ScopeLevel> scopeLevels = this->scopeLevels;

    while (!scopeLevels.empty()) {
        shared_ptr<ValueType> type = scopeLevels.top().functionReturnTypeMap[name];
        if (type != nullptr)
            return type;
        scopeLevels.pop();
    }

    return nullptr;
}

bool AnalyzerScope::setFunctionReturnType(string name, shared_ptr<ValueType> returnType) {
    if (scopeLevels.top().functionReturnTypeMap[name] != nullptr)
        return false;

    scopeLevels.top().functionReturnTypeMap[name] = returnType;
    return true;
}
