#ifndef STATEMENT_FUNCTION_DECLARATION_H
#define STATEMENT_FUNCTION_DECLARATION_H

#include <format>

#include "Statement.h"

class ValueType;
class ValueTypeFun;

enum class FunctionKind {
    MODULE,
    BLOB,
    PROTO
};

class StatementFunctionDeclaration: public Statement {
friend class Parser;

public:
    StatementFunctionDeclaration(
        bool isVisibleOutside,
        FunctionKind kind,
        const std::string &name,
        const std::string &moduleName,
        const std::vector<std::pair<std::string, std::shared_ptr<ValueType>>> &arguments,
        std::shared_ptr<ValueType> returnValueType,
        std::shared_ptr<Location> location
    );

    bool getIsVisibleOutside() const;
    FunctionKind getKind() const;
    std::string getName() const;
    std::string getGlobalName() const;
    std::string getModuleName() const;
    void setModuleName(const std::string &moduleName);
    std::vector<std::pair<std::string, std::shared_ptr<ValueType>>> getArguments() const;
    std::shared_ptr<ValueType> getReturnValueType() const;
    std::shared_ptr<ValueTypeFun> getValueType() const;

private:
    bool isVisibleOutside;
    FunctionKind kind;
    std::string name;
    std::string moduleName;

    std::vector<std::pair<std::string, std::shared_ptr<ValueType>>> arguments;
    std::shared_ptr<ValueType> returnValueType;
};

#endif