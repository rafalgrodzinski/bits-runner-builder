#include "AnalyzerScope.h"
#include "Parser/ValueType/ValueType.h"
#include "Parser/SymbolName.h"

//
// AnalyzerScopeBoxed
//
/*AnalyzerScopeBoxed::AnalyzerScopeBoxed(AnalyzerScope *parent):
parent(parent) { }

shared_ptr<ValueType> AnalyzerScopeBoxed::getNamedValueType(const string &namedValueTypeKey) const {
    stack<AnalyzerScope::ScopeLevel> scopeLevels = parent->scopeLevels;

    while (!scopeLevels.empty()) {
        auto it = scopeLevels.top().boxedNamedValueTypesMap.find(namedValueTypeKey);
        if (it != scopeLevels.top().boxedNamedValueTypesMap.end())
            return scopeLevels.top().boxedNamedValueTypesMap[namedValueTypeKey];
        scopeLevels.pop();
    }

    return nullptr;
}

AnalyzerScopeRegisterResult AnalyzerScopeBoxed::registerNamedValueTypesMap(const vector<string> &namedValueTypeKeys, const vector<shared_ptr<ValueType>> &namedValueTypes) {
    for (int i=0; i<namedValueTypeKeys.size(); i++) {
        parent->scopeLevels.top().boxedNamedValueTypesMap[namedValueTypeKeys[i]] = namedValueTypes[i];
    }

    return AnalyzerScopeRegisterResult::SUCCES;
}*/

//
// AnalyzerScopeEnum
//
/*AnalyzerScopeEnum::AnalyzerScopeEnum(AnalyzerScope *parent):
parent(parent) { }

optional<vector<string>> AnalyzerScopeEnum::getNamedValueTypeKeys(shared_ptr<SymbolName> symbolName) {
    stack<AnalyzerScope::ScopeLevel> scopeLevels = parent->scopeLevels;

    while (!scopeLevels.empty()) {
        auto it = scopeLevels.top().enumNamedValueTypeKeys.find(*symbolName);
        if (it != scopeLevels.top().enumNamedValueTypeKeys.end())
            return scopeLevels.top().enumNamedValueTypeKeys[*symbolName];
        scopeLevels.pop();
    }

    return {};
}

AnalyzerScopeRegisterResult AnalyzerScopeEnum::registerNamedValueTypeKeys(shared_ptr<SymbolName> symbolName, const vector<string> &namedValueTypeKeys) {
    optional<vector<string>> &oDeclaredNamedValueTypeKeys = parent->scopeLevels.top().enumNamedValueTypeKeys[*symbolName];

    // If it's already declared, check
    if (oDeclaredNamedValueTypeKeys) {
        vector<string> &declaredNamedValueTypeKeys = *oDeclaredNamedValueTypeKeys;
        for (const string &namedValueTypeKey : namedValueTypeKeys) {
            // first check if each of the named types is not yet declared
            for (string &declaredNamedValueTypeKey : declaredNamedValueTypeKeys) {
                if (declaredNamedValueTypeKey == namedValueTypeKey)
                    return AnalyzerScopeRegisterResult::FAILURE_ALREADY_DECLARED;
            }
        }
    } else {
        oDeclaredNamedValueTypeKeys = vector<string>();
    }

    for (const string &namedValueTypeKey : namedValueTypeKeys) {
        (*oDeclaredNamedValueTypeKeys).push_back(namedValueTypeKey);
    }

    return AnalyzerScopeRegisterResult::SUCCES;
}

shared_ptr<ValueType> AnalyzerScopeEnum::getPayloadValueType(shared_ptr<SymbolName> symbolName) {
    stack<AnalyzerScope::ScopeLevel> scopeLevels = parent->scopeLevels;

    while (!scopeLevels.empty()) {
        auto it = scopeLevels.top().enumPayloadValueType.find(*symbolName);
        if (it != scopeLevels.top().enumPayloadValueType.end())
            return scopeLevels.top().enumPayloadValueType[*symbolName];
        scopeLevels.pop();
    }

    return nullptr;
}

AnalyzerScopeRegisterResult AnalyzerScopeEnum::registerPayloadValueType(shared_ptr<SymbolName> symbolName, shared_ptr<ValueType> payloadValueType) {
    parent->scopeLevels.top().enumPayloadValueType[*symbolName] = payloadValueType;
    return AnalyzerScopeRegisterResult::SUCCES;
}*/

//
// AnalyzerScope
//

AnalyzerScope::AnalyzerScope() {
    blobScope = make_shared<AnalyzerScopeBlob>(this);
    boxedScope = make_shared<AnalyzerScopeBoxed>(this);
    enumScope = make_shared<AnalyzerScopeEnum>(this);

    pushLevel();
}

void AnalyzerScope::pushLevel() {
    scopeLevels.push(ScopeLevel());
}

void AnalyzerScope::popLevel() {
    scopeLevels.pop();
}

optional<vector<pair<string, shared_ptr<ValueType>>>> AnalyzerScope::getProtoMembers(const string &name) const {
    stack<ScopeLevel> scopeLevels = this->scopeLevels;

    while (!scopeLevels.empty()) {
        auto it = scopeLevels.top().protoMembersMap.find(name);
        if (it != scopeLevels.top().protoMembersMap.end())
            return scopeLevels.top().protoMembersMap[name];
        scopeLevels.pop();
    }

    return {};
}

bool AnalyzerScope::setProtoMembers(const string &name, const optional<vector<pair<string, shared_ptr<ValueType>>>> &members) {
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

optional<vector<pair<string, shared_ptr<ValueType>>>> AnalyzerScope::getBlobMembers(shared_ptr<ValueType> valueType) const {
    string blobName = valueType->getGlobalName();
    stack<ScopeLevel> scopeLevels = this->scopeLevels;

    while (!scopeLevels.empty()) {
        auto it = scopeLevels.top().blobMembersMap.find(blobName);
        // check if found members
        if (it != scopeLevels.top().blobMembersMap.end()) {
            vector<pair<string, shared_ptr<ValueType>>> blobMembers = *scopeLevels.top().blobMembersMap[blobName];
            // update named value types
            for (pair<string, shared_ptr<ValueType>> &blobMember : blobMembers) {
                if (blobMember.second->isBoxed()) {
                    blobMember.second->namedTypeKeys = valueType->getNamedTypeKeys();
                    blobMember.second->namedTypeValues = valueType->getNamedTypeValues();
                }
            }
            return blobMembers;
        }
        scopeLevels.pop();
    }

    return {};
}

optional<vector<shared_ptr<ValueType>>> AnalyzerScope::getNonFunctionBlobMemberTypes(shared_ptr<ValueType> valueType) const {
    optional<vector<pair<string, shared_ptr<ValueType>>>> blobMembers = getBlobMembers(valueType);
        if (!blobMembers)
            return { };

    vector<shared_ptr<ValueType>> targetMemberTypes;
    for (pair<string, shared_ptr<ValueType>> &member : *blobMembers) {
        if (!member.second->isFunction())
            targetMemberTypes.push_back(member.second);
    }

    return targetMemberTypes;
}

bool AnalyzerScope::isBlobDeclared(const string &name) const{
    stack<ScopeLevel> scopeLevels = this->scopeLevels;

    while (!scopeLevels.empty()) {
        auto it = scopeLevels.top().blobMembersMap.find(name);
        if (it != scopeLevels.top().blobMembersMap.end())
            return true;
        scopeLevels.pop();
    }

    return false;
}

bool AnalyzerScope::setBlobMembers(const string &name, const optional<vector<pair<string, shared_ptr<ValueType>>>> &members) {
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

bool AnalyzerScope::isNamedTypeDeclared(const string &namedType) const {
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

bool AnalyzerScope::setNamedTypes(const vector<string> &namedTypes) {
    for (const string &namedType : namedTypes) {
        // first check if each of the named types is not yet declared
        for (string &declaredNamedType : scopeLevels.top().namedTypes) {
            if (declaredNamedType.compare(namedType) == 0)
                return false;
        }
        scopeLevels.top().namedTypes.push_back(namedType);
    }
    return true;
}

optional<vector<string>> AnalyzerScope::getBlobProtoNames(const string &name) const {
    stack<ScopeLevel> scopeLevels = this->scopeLevels;

    while (!scopeLevels.empty()) {
        auto it = scopeLevels.top().blobProtosMap.find(name);
        if (it != scopeLevels.top().blobProtosMap.end())
            return scopeLevels.top().blobProtosMap[name];
        scopeLevels.pop();
    }

    return {};
}

bool AnalyzerScope::setBlobProtoNames(const string &name, const vector<string> &protoNames) {
    scopeLevels.top().blobProtosMap[name] = protoNames;

    return true;
}

shared_ptr<ValueType> AnalyzerScope::getVariableType(const string &identifier) const {
    stack<ScopeLevel> scopeLevels = this->scopeLevels;

    while (!scopeLevels.empty()) {
        shared_ptr<ValueType> type = scopeLevels.top().variableTypes[identifier];
        if (type != nullptr)
            return type;
        scopeLevels.pop();
    }

    return nullptr;
}

bool AnalyzerScope::setVariableType(const string &identifier, shared_ptr<ValueType> type, bool isDefinition) {
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

shared_ptr<ValueType> AnalyzerScope::getFunctionType(const string &name) const {
    stack<ScopeLevel> scopeLevels = this->scopeLevels;

    while (!scopeLevels.empty()) {
        shared_ptr<ValueType> type = scopeLevels.top().functionTypeMap[name];
        if (type != nullptr)
            return type;
        scopeLevels.pop();
    }

    return nullptr;
}

bool AnalyzerScope::setFunctionType(const string &name, shared_ptr<ValueType> type, bool isDefinition) {
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