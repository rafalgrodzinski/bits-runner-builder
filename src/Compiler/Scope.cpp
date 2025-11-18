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

bool Scope::setAlloca(string identifier, llvm::AllocaInst *alloca) {
    if (scopeLevels.top().allocaMap[identifier] != nullptr)
        return false;

    scopeLevels.top().allocaMap[identifier] = alloca;
    return true;
}

llvm::AllocaInst *Scope::getAlloca(string identifier) {
    stack<ScopeLevel> scopeLevels = this->scopeLevels;

    while (!scopeLevels.empty()) {
        llvm::AllocaInst *alloca = scopeLevels.top().allocaMap[identifier];
        if (alloca != nullptr)
            return alloca;
        scopeLevels.pop();
    }

    return nullptr;
}

bool Scope::setFunction(string name, llvm::Function *function) {
    if (scopeLevels.top().funMap[name] != nullptr)
        return false;

    scopeLevels.top().funMap[name] = function;
    return true;
}

llvm::Function* Scope::getFunction(string name) {
    stack<ScopeLevel> scopeLevels = this->scopeLevels;

    while (!scopeLevels.empty()) {
        llvm::Function *function = scopeLevels.top().funMap[name];
        if (function != nullptr)
            return function;
        scopeLevels.pop();
    }

    return nullptr;
}

bool Scope::setInlineAsm(string name, llvm::InlineAsm *inlineAsm) {
    if (scopeLevels.top().rawFunMap[name] != nullptr)
        return false;
    
    scopeLevels.top().rawFunMap[name] = inlineAsm;
    return true;
}

llvm::InlineAsm *Scope::getInlineAsm(string name) {
    stack<ScopeLevel> scopeLevels = this->scopeLevels;

    while (!scopeLevels.empty()) {
        llvm::InlineAsm *inlineAsm = scopeLevels.top().rawFunMap[name];
        if (inlineAsm != nullptr)
            return inlineAsm;
        scopeLevels.pop();
    }

    return nullptr;
}

bool Scope::setPtrType(string identifier, shared_ptr<ValueType> ptrType) {
    if (scopeLevels.top().ptrTypeMap[identifier] != nullptr)
        return false;

    scopeLevels.top().ptrTypeMap[identifier] = ptrType;
    return true;
}

shared_ptr<ValueType> Scope::getPtrType(string name) {
    stack<ScopeLevel> scopeLevels = this->scopeLevels;

    while (!scopeLevels.empty()) {
        shared_ptr<ValueType> ptrType = scopeLevels.top().ptrTypeMap[name];
        if (ptrType != nullptr)
            return ptrType;
        scopeLevels.pop();
    }

    return nullptr;
}

bool Scope::setGlobal(string identifier, llvm::Value *global) {
    if (scopeLevels.top().globalMap[identifier] != nullptr)
        return false;

    scopeLevels.top().globalMap[identifier] = global;
    return true;
}

llvm::Value *Scope::getGlobal(string identifier) {
    stack<ScopeLevel> scopeLevels = this->scopeLevels;

    while (!scopeLevels.empty()) {
        llvm::Value *global = scopeLevels.top().globalMap[identifier];
        if (global != nullptr)
            return global;
        scopeLevels.pop();
    }

    return nullptr;
}

bool Scope::setStruct(string structName, llvm::StructType *structType, vector<string> memberNames) {
    scopeLevels.top().structTypeMap[structName] = structType;
    scopeLevels.top().structMembersMap[structName] = memberNames;

    return true;
}

llvm::StructType *Scope::getStructType(string structName) {
    stack<ScopeLevel> scopeLevels = this->scopeLevels;

    while (!scopeLevels.empty()) {
        llvm::StructType *structType = scopeLevels.top().structTypeMap[structName];
        if (structType != nullptr)
            return structType;
        scopeLevels.pop();
    }

    return nullptr;
}

optional<int> Scope::getStructMemberIndex(string structName, string memberName) {
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