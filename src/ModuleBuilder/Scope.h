#ifndef SCOPE_H
#define SCOPE_H

#include <map>
#include <stack>
#include <string>

#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>

class ValueType;
class WrappedValue;

using namespace std;

class Scope {
private:
    typedef struct {
        map<string, shared_ptr<WrappedValue>> wrappedValueMap;
        map<string, llvm::Function*> funMap;
        map<string, llvm::InlineAsm*> rawFunMap;
        map<string, llvm::StructType*> structTypeMap;
        map<string, vector<string>> structMembersMap;
    } ScopeLevel;

    stack<ScopeLevel> scopeLevels;

public:
    Scope();

    void pushLevel();
    void popLevel();

    bool setWrappedValue(string identifier, shared_ptr<WrappedValue> wrappedvalue);
    shared_ptr<WrappedValue> getWrappedValue(string identifier);

    bool setFunction(string name, llvm::Function *fun);
    llvm::Function *getFunction(string name);

    bool setInlineAsm(string name, llvm::InlineAsm *inlineAsm);
    llvm::InlineAsm *getInlineAsm(string name);

    bool setStruct(string structName, llvm::StructType *type, vector<string> memberNames);
    llvm::StructType *getStructType(string memberName);
    optional<int> getStructMemberIndex(string structName, string memberName);
};

#endif