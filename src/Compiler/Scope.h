#ifndef SCOPE_H
#define SCOPE_H

#include <map>
#include <string>
#include <stack>

#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>

class ValueType;

using namespace std;

class Scope {
private:
    typedef struct {
        map<string, llvm::AllocaInst*> allocaMap;
        map<string, llvm::Function*> funMap;
        map<string, llvm::InlineAsm*> rawFunMap;
        map<string, llvm::StructType*> structTypeMap;
        map<string, vector<string>> structMembersMap;
        map<string, llvm::Value*> globalMap;
    } ScopeLevel;

    stack<ScopeLevel> scopeLevels;

public:
    Scope();

    void pushLevel();
    void popLevel();

    bool setAlloca(string identifier, llvm::AllocaInst *alloca);
    llvm::AllocaInst *getAlloca(string identifier);

    bool setFunction(string name, llvm::Function *fun);
    llvm::Function *getFunction(string name);

    bool setInlineAsm(string name, llvm::InlineAsm *inlineAsm);
    llvm::InlineAsm *getInlineAsm(string name);

    bool setGlobal(string identifier, llvm::Value *global);
    llvm::Value *getGlobal(string identifier);

    bool setStruct(string structName, llvm::StructType *type, vector<string> memberNames);
    llvm::StructType *getStructType(string memberName);
    optional<int> getStructMemberIndex(string structName, string memberName);
};

#endif