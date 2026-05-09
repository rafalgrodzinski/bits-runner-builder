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

optional<vector<pair<string, shared_ptr<ValueType>>>> AnalyzerScope::getBlobMembers(shared_ptr<ValueType> blobValueType) {
    optional<string> blobName = blobValueType->getBlobName();
    if (!blobName)
        return {};
    stack<ScopeLevel> scopeLevels = this->scopeLevels;

    while (!scopeLevels.empty()) {
        auto it = scopeLevels.top().blobMembersMap.find(*blobName);
        // check if found members
        if (it != scopeLevels.top().blobMembersMap.end()) {
            vector<pair<string, shared_ptr<ValueType>>> blobMembers = *scopeLevels.top().blobMembersMap[*blobName];
            // update named value types
            for (pair<string, shared_ptr<ValueType>> &blobMember : blobMembers) {
                blobMember.second->namedTypeKeys = blobValueType->getNamedTypeKeys();
                blobMember.second->namedTypeValues = blobValueType->getNamedTypeValues();
            }
            return blobMembers;
        }
        scopeLevels.pop();
    }

    return {};
}

optional<vector<shared_ptr<ValueType>>> AnalyzerScope::getNonFunctionBlobMemberTypes(shared_ptr<ValueType> blobValueType) {
    optional<vector<pair<string, shared_ptr<ValueType>>>> blobMembers = getBlobMembers(blobValueType);
        if (!blobMembers)
            return { };

    vector<shared_ptr<ValueType>> targetMemberTypes;
    for (pair<string, shared_ptr<ValueType>> &member : *blobMembers) {
        if (!member.second->isFunction())
            targetMemberTypes.push_back(member.second);
    }

    return targetMemberTypes;
}

bool AnalyzerScope::isBlobDeclared(string name) {
    stack<ScopeLevel> scopeLevels = this->scopeLevels;

    while (!scopeLevels.empty()) {
        auto it = scopeLevels.top().blobMembersMap.find(name);
        if (it != scopeLevels.top().blobMembersMap.end())
            return true;
        scopeLevels.pop();
    }

    return false;
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

bool AnalyzerScope::isNamedTypeDeclared(string namedType) {
    stack<ScopeLevel> scopeLevels = this->scopeLevels;

    while (!scopeLevels.empty()) {
        for (string &declaredNamedType : scopeLevels.top().namedTypes) {
            if (declaredNamedType.compare(namedType) == 0)
                return true;
        }
        scopeLevels.pop();
    }

    return false;
}

bool AnalyzerScope::setNamedTypes(vector<string> namedTypes) {
    for (string &namedType : namedTypes) {
        // first check if each of the named types is not yet declared
        for (string &declaredNamedType : scopeLevels.top().namedTypes) {
            if (declaredNamedType.compare(namedType) == 0)
                return false;
        }
        scopeLevels.top().namedTypes.push_back(namedType);
    }
    return true;
}

optional<vector<string>> AnalyzerScope::getBlobNamedTypeKeys(string blobName) {
    stack<ScopeLevel> scopeLevels = this->scopeLevels;

    while (!scopeLevels.empty()) {
        auto it = scopeLevels.top().blobNamedTypeKeysMap.find(blobName);
        if (it != scopeLevels.top().blobNamedTypeKeysMap.end())
            return scopeLevels.top().blobNamedTypeKeysMap[blobName];
        scopeLevels.pop();
    }

    return {};
}

bool AnalyzerScope::setBlobNamedTypeKeys(string blobName, vector<string> namedTypeKeys) {
    // check if named types are already defined
    if (scopeLevels.top().blobNamedTypeKeysMap.find(blobName) != scopeLevels.top().blobNamedTypeKeysMap.end())
        return false;

    scopeLevels.top().blobNamedTypeKeysMap[blobName] = namedTypeKeys;

    return true;
}

optional<vector<string>> AnalyzerScope::getBlobProtoNames(string name) {
    stack<ScopeLevel> scopeLevels = this->scopeLevels;

    while (!scopeLevels.empty()) {
        auto it = scopeLevels.top().blobProtosMap.find(name);
        if (it != scopeLevels.top().blobProtosMap.end())
            return scopeLevels.top().blobProtosMap[name];
        scopeLevels.pop();
    }

    return {};
}

bool AnalyzerScope::setBlobProtoNames(string name, vector<string> protoNames) {
    scopeLevels.top().blobProtosMap[name] = protoNames;

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
