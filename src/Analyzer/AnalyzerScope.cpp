#include "AnalyzerScope.h"
#include "Parser/ValueType/ValueType.h"
#include "Parser/SymbolName.h"

AnalyzerScope::AnalyzerScope() {
    blobScope = make_shared<AnalyzerScopeBlob>(this);
    boxedScope = make_shared<AnalyzerScopeBoxed>(this);
    enumScope = make_shared<AnalyzerScopeEnum>(this);
    protoScope = make_shared<AnalyzerScopeProto>(this);

    pushLevel();
}

void AnalyzerScope::pushLevel() {
    scopeLevels.push(ScopeLevel());
}

void AnalyzerScope::popLevel() {
    scopeLevels.pop();
}

void AnalyzerScope::level(function<void()> levelBlock) {
    pushLevel();
    levelBlock();
    popLevel();
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

        /*if (existingType->getSubType() != nullptr && type->getSubType() != nullptr) {
            if (existingType->getSubType()->getKind() != type->getSubType()->getKind())
                return false;
        }*/
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
        /*if (!existingType->isEqual(type))
            return false;*/
    }

    scopeLevels.top().functionTypeMap[name] = type;
    if (isDefinition)
        scopeLevels.top().isFunctionDefinedMap[name] = true;

    return true;
}