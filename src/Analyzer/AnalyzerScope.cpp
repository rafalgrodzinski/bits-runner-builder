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

bool AnalyzerScope::setVariableType(string identifier, shared_ptr<ValueType> type, bool isDefinition) {
    shared_ptr<ValueType> existingType = scopeLevels.top().variableTypes[identifier];
    if (existingType != nullptr) {
        // defining already defined variable
        if (scopeLevels.top().isVariableDefinedMap[identifier] && isDefinition)
            return false;

        // check if kind and subtypes' kinds match (ignore count expression since it may not be defined for declarations)
        if (existingType->getKind() != type->getKind())
            return false;

        if (existingType->getSubType() != nullptr && type->getSubType() != nullptr) {
            if (existingType->getSubType()->getKind() != type->getSubType()->getKind())
                return false;
        }
    }

    scopeLevels.top().variableTypes[identifier] = type;
    if (isDefinition)
        scopeLevels.top().isVariableDefinedMap[identifier] = true;

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

bool AnalyzerScope::setBlobMembers(string name, vector<pair<string, shared_ptr<ValueType>>> members, bool isDefinition) {
    bool isDefined = scopeLevels.top().isBlobDefinedMap[name];

    // defining already defined blob
    if (isDefined && isDefinition)
        return false;

    if (!isDefined)
        scopeLevels.top().blobMembersMap[name] = members;

    if (isDefinition)
        scopeLevels.top().isBlobDefinedMap[name] = true;

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
