#ifndef SCOPE_H
#define SCOPE_H

#include <map>
#include <stack>
#include <string>

#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>

class ValueType;
class WrappedValue;

class Scope {
public:
    Scope();

    void pushLevel();
    void popLevel();

    bool setWrappedValue(const std::string &identifier, std::shared_ptr<WrappedValue> wrappedvalue);
    std::shared_ptr<WrappedValue> getWrappedValue(const std::string &identifier);

    bool setFunction(const std::string &name, llvm::Function *fun);
    llvm::Function *getFunction(const std::string &name) const;

    bool setInlineAsm(const std::string &name, llvm::InlineAsm *inlineAsm);
    llvm::InlineAsm *getInlineAsm(const std::string &name) const;

    bool setEnumFieldTagValue(const std::string &enumFieldName, llvm::Constant *enumTagValue);
    llvm::Constant *getEnumFieldTagValue(const std::string &enumFieldName) const;

    bool setProtoStructType(const std::string &name, llvm::StructType *structType, const std::vector<std::pair<std::string, std::shared_ptr<ValueType>>> &members);
    llvm::StructType *getProtoStructType(const std::string &name) const;
    std::optional<std::vector<std::pair<std::string, std::shared_ptr<ValueType>>>> getProtoStructMembers(const std::string &protoName) const;

    bool setStruct(const std::string &structName, llvm::StructType *type, const std::vector<std::string> &memberNames);
    llvm::StructType *getStructType(const std::string &memberName) const;
    std::optional<int> getStructMemberIndex(const std::string &structName, const std::string &memberName) const;

private:
    typedef struct {
        std::map<std::string, std::shared_ptr<WrappedValue>> wrappedValueMap;
        std::map<std::string, llvm::Function*> funMap;
        std::map<std::string, llvm::InlineAsm*> rawFunMap;

        std::map<std::string, llvm::Constant*> enumFieldTagValuesMap;

        std::map<std::string, llvm::StructType*> protoStructTypesMap;
        std::map<std::string, std::vector<std::pair<std::string, std::shared_ptr<ValueType>>>> protoStructMembersMap;

        std::map<std::string, llvm::StructType*> structTypeMap;
        std::map<std::string, std::vector<std::string>> structMembersMap;
    } ScopeLevel;

    std::stack<ScopeLevel> scopeLevels;
};

#endif