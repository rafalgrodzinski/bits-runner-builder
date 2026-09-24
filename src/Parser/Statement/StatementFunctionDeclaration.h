#ifndef STATEMENT_FUNCTION_DECLARATION_H
#define STATEMENT_FUNCTION_DECLARATION_H

#include <format>

#include "Statement.h"

class ValueType;
class ValueTypeFun;

class StatementFunctionDeclaration: public Statement {
friend class Parser;

private:
    bool shouldExport;
    std::string name;
    std::string moduleName;

    std::vector<std::pair<std::string, std::shared_ptr<ValueType>>> arguments;
    std::shared_ptr<ValueType> returnValueType;

public:
    StatementFunctionDeclaration(
        bool shouldExport,
        const std::string &name,
        const std::string &moduleName,
        const std::vector<std::pair<std::string, std::shared_ptr<ValueType>>> &arguments,
        std::shared_ptr<ValueType> returnValueType,
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
};

#endif