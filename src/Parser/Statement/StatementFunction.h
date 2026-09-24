#ifndef STATEMENT_FUNCTION_H
#define STATEMENT_FUNCTION_H

#include <format>

#include "Statement.h"
#include "StatementFunctionDeclaration.h"

class StatementBlock;
class ValueType;
class ValueTypeFun;

class StatementFunction: public Statement {
friend class Parser;

public:
    StatementFunction(
        bool shouldExport,
        const std::string &name,
        const std::vector<std::pair<std::string, std::shared_ptr<ValueType>>> &arguments,
        std::shared_ptr<ValueType> returnValueType,
        std::shared_ptr<StatementBlock> statementBlock,
        std::shared_ptr<Location> location
    );

    bool getShouldExport() const;
    std::string getName() const;
    std::string getGlobalName() const;
    std::string getModuleName() const;
    void setModuleName(const std::string &moduleName);
    std::vector<std::pair<std::string, std::shared_ptr<ValueType>>> getArguments() const;
    std::shared_ptr<ValueType> getReturnValueType() const;
    std::shared_ptr<ValueTypeFun> getValueType() const;
    std::shared_ptr<StatementBlock> getStatementBlock() const;
    std::shared_ptr<StatementFunctionDeclaration> getDeclaration() const;

private:
    bool shouldExport;
    std::string name;
    std::string moduleName;
    std::vector<std::pair<std::string, std::shared_ptr<ValueType>>> arguments;
    std::shared_ptr<ValueType> returnValueType;
    std::shared_ptr<StatementBlock> statementBlock;
};

#endif