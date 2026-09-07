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

        map<string, llvm::Constant*> enumFieldTagValuesMap;

        map<string, llvm::StructType*> protoStructTypesMap;
        map<string, vector<pair<string, shared_ptr<ValueType>>>> protoStructMembersMap;

        map<string, llvm::StructType*> structTypeMap;
        map<string, vector<string>> structMembersMap;
    } ScopeLevel;

    stack<ScopeLevel> scopeLevels;

public:
    Scope();

    void pushLevel();
    void popLevel();

    bool setWrappedValue(const string &identifier, shared_ptr<WrappedValue> wrappedvalue);
    shared_ptr<WrappedValue> getWrappedValue(const string &identifier);

    bool setFunction(const string &name, llvm::Function *fun);
    llvm::Function *getFunction(const string &name) const;

    bool setInlineAsm(const string &name, llvm::InlineAsm *inlineAsm);
    llvm::InlineAsm *getInlineAsm(const string &name) const;

    bool setEnumFieldTagValue(const string &enumFieldName, llvm::Constant *enumTagValue);
    llvm::Constant *getEnumFieldTagValue(const string &enumFieldName) const;

    bool setProtoStructType(const string &name, llvm::StructType *structType, const vector<pair<string, shared_ptr<ValueType>>> &members);
    llvm::StructType *getProtoStructType(const string &name) const;
    optional<vector<pair<string, shared_ptr<ValueType>>>> getProtoStructMembers(const string &protoName) const;

    bool setStruct(const string &structName, llvm::StructType *type, const vector<string> &memberNames);
    llvm::StructType *getStructType(const string &memberName) const;
    optional<int> getStructMemberIndex(const string &structName, const string &memberName) const;
};

#endif