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

optional<vector<pair<string, shared_ptr<ValueType>>>> AnalyzerScope::getProtoMembers(string name) {
    stack<ScopeLevel> scopeLevels = this->scopeLevels;

    while (!scopeLevels.empty()) {
        auto it = scopeLevels.top().protoMembersMap.find(name);
        if (it != scopeLevels.top().protoMembersMap.end())
            return scopeLevels.top().protoMembersMap[name];
        scopeLevels.pop();
    }

    return {};
}

bool AnalyzerScope::setProtoMembers(string name, optional<vector<pair<string, shared_ptr<ValueType>>>> members) {
    bool isDefinition = members.has_value();
    bool isDefined = false;
    if (scopeLevels.top().protoMembersMap.find(name) != scopeLevels.top().protoMembersMap.end())
        isDefined = scopeLevels.top().protoMembersMap[name].has_value();

    // defining already defined proto
    if (isDefined && isDefinition)
        return false;

    if (!isDefined)
        scopeLevels.top().protoMembersMap[name] = members;

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

bool AnalyzerScope::setBlobMembers(string name, optional<vector<pair<string, shared_ptr<ValueType>>>> members) {
    bool isDefinition = members.has_value();
    bool isDefined = false;
    if (scopeLevels.top().blobMembersMap.find(name) != scopeLevels.top().blobMembersMap.end())
        isDefined = scopeLevels.top().blobMembersMap[name].has_value();

    // defining already defined blob
    if (isDefined && isDefinition)
        return false;

    if (!isDefined)
        scopeLevels.top().blobMembersMap[name] = members;

    return true;
}

optional<vector<string>> AnalyzerScope::getBlobProtoNames(string name) {
    stack<ScopeLevel> scopeLevels = this->scopeLevels;

    while (!scopeLevels.empty()) {
        auto it = scopeLevels.top().blobProtosmMap.find(name);
        if (it != scopeLevels.top().blobProtosmMap.end())
            return scopeLevels.top().blobProtosmMap[name];
        scopeLevels.pop();
    }

    return {};
}

bool AnalyzerScope::setBlobProtoNames(string name, vector<string> protoNames) {
    scopeLevels.top().blobProtosmMap[name] = protoNames;

    return true;
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
