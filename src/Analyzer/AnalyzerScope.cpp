#include "AnalyzerScope.h"

#include "Parser/ValueType.h"

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

optional<vector<pair<string, shared_ptr<ValueType>>>> AnalyzerScope::getBlobMembers(string name) {
    stack<ScopeLevel> scopeLevels = this->scopeLevels;

    while (!scopeLevels.empty()) {
        auto it = scopeLevels.top().blobMembersMap.find(name);
        if (it != scopeLevels.top().blobMembersMap.end())
            return scopeLevels.top().blobMembersMap[name];
        scopeLevels.pop();
    }

    return {};
}

bool AnalyzerScope::setBlobMembers(string name, vector<pair<string, shared_ptr<ValueType>>> members) {
    auto it = scopeLevels.top().blobMembersMap.find(name);
    if (it != scopeLevels.top().blobMembersMap.end())
        return false;

    scopeLevels.top().blobMembersMap[name] = members;
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

bool AnalyzerScope::setFunctionType(string name, shared_ptr<ValueType> type, bool isDefinition) {
    shared_ptr<ValueType> existingType = scopeLevels.top().functionTypeMap[name];
    if (existingType != nullptr) {
        // defining already defined function
        if (scopeLevels.top().isFunctionDefinedMap[name] && isDefinition)
            return false;
        // type doesn't match existing type
        if (!existingType->isEqual(type))
            return false;
    }

    scopeLevels.top().functionTypeMap[name] = type;
    if (isDefinition)
        scopeLevels.top().isFunctionDefinedMap[name] = true;

    return true;
}
