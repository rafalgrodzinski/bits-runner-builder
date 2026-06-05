#include "Scope.h"

Scope::Scope() {
    pushLevel();
}

void Scope::pushLevel() {
    scopeLevels.push(ScopeLevel());
}

void Scope::popLevel() {
    scopeLevels.pop();
}

bool Scope::setWrappedValue(const string &identifier, shared_ptr<WrappedValue> wrappedValue) {
    if (scopeLevels.top().wrappedValueMap[identifier] != nullptr)
        return false;

    scopeLevels.top().wrappedValueMap[identifier] = wrappedValue;
    return true;
}

shared_ptr<WrappedValue> Scope::getWrappedValue(const string &identifier) {
    stack<ScopeLevel> scopeLevels = this->scopeLevels;

    while (!scopeLevels.empty()) {
        shared_ptr<WrappedValue> wrappedValue = scopeLevels.top().wrappedValueMap[identifier];
        if (wrappedValue != nullptr)
            return wrappedValue;
        scopeLevels.pop();
    }

    return nullptr;
}

bool Scope::setFunction(const string &name, llvm::Function *function) {
    if (scopeLevels.top().funMap[name] != nullptr)
        return false;

    scopeLevels.top().funMap[name] = function;
    return true;
}

llvm::Function* Scope::getFunction(const string &name) const {
    stack<ScopeLevel> scopeLevels = this->scopeLevels;

    while (!scopeLevels.empty()) {
        llvm::Function *function = scopeLevels.top().funMap[name];
        if (function != nullptr)
            return function;
        scopeLevels.pop();
    }

    return nullptr;
}

bool Scope::setInlineAsm(const string &name, llvm::InlineAsm *inlineAsm) {
    if (scopeLevels.top().rawFunMap[name] != nullptr)
        return false;
    
    scopeLevels.top().rawFunMap[name] = std::move(inlineAsm);
    return true;
}

llvm::InlineAsm *Scope::getInlineAsm(const string &name) const {
    stack<ScopeLevel> scopeLevels = this->scopeLevels;

    while (!scopeLevels.empty()) {
        llvm::InlineAsm *inlineAsm = scopeLevels.top().rawFunMap[name];
        if (inlineAsm != nullptr)
            return inlineAsm;
        scopeLevels.pop();
    }

    return nullptr;
}

bool Scope::setProtoStructType(const string &name, llvm::StructType *structType, const vector<pair<string, shared_ptr<ValueType>>> &members) {
    scopeLevels.top().protoStructTypesMap[name] = structType;
    scopeLevels.top().protoStructMembersMap[name] = std::move(members);

    return true;
}

llvm::StructType *Scope::getProtoStructType(const string &name) const {
    stack<ScopeLevel> scopeLevels = this->scopeLevels;

    while (!scopeLevels.empty()) {
        llvm::StructType *protoStruct = scopeLevels.top().protoStructTypesMap[name];
        if (protoStruct != nullptr)
            return protoStruct;
        scopeLevels.pop();
    }

    return nullptr;
}

optional<vector<pair<string, shared_ptr<ValueType>>>> Scope::getProtoStructMembers(const string &protoName) const {
    stack<ScopeLevel> scopeLevels = this->scopeLevels;

    while (!scopeLevels.empty()) {
        if (scopeLevels.top().protoStructMembersMap.contains(protoName))
            return scopeLevels.top().protoStructMembersMap[protoName];
        scopeLevels.pop();
    }

    return {};
}

bool Scope::setStruct(const string &structName, llvm::StructType *structType, const vector<string> &memberNames) {
    scopeLevels.top().structTypeMap[structName] = structType;
    scopeLevels.top().structMembersMap[structName] = std::move(memberNames);

    return true;
}

llvm::StructType *Scope::getStructType(const string &structName) const {
    stack<ScopeLevel> scopeLevels = this->scopeLevels;

    while (!scopeLevels.empty()) {
        llvm::StructType *structType = scopeLevels.top().structTypeMap[structName];
        if (structType != nullptr)
            return structType;
        scopeLevels.pop();
    }

    return nullptr;
}

optional<int> Scope::getStructMemberIndex(const string &structName, const string &memberName) const {
        stack<ScopeLevel> scopeLevels = this->scopeLevels;

    while (!scopeLevels.empty()) {
        if (scopeLevels.top().structMembersMap.contains(structName)) {
            vector<string> memberNames = scopeLevels.top().structMembersMap[structName];
            for (int i=0; i<memberNames.size(); i++) {
                if (memberNames[i].compare(memberName) == 0)
                    return i;
            }
        }
        scopeLevels.pop();
    }

    return {};
}