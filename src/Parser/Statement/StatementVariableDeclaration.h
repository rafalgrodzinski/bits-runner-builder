#ifndef STATEMENT_VARIABLE_DECLARATION_H
#define STATEMENT_VARIABLE_DECLARATION_H

#include <format>

#include "Statement.h"

class ValueType;

class StatementVariableDeclaration: public Statement {
friend class Analyzer;

public:
    StatementVariableDeclaration(
        const std::string &identifier,
        std::shared_ptr<ValueType> valueType,
        std::shared_ptr<Location> location
    );

    StatementVariableDeclaration(
        bool shouldExport,
        const std::string &identifier,
        const std::string &moduleName,
        bool isRoot,
        std::shared_ptr<ValueType> valueType,
        std::shared_ptr<Location> location
    );

    bool getShouldExport() const;
    std::string getIdentifier() const;
    std::string getGlobalIdentifier() const;
    std::string getModuleName() const;
    void setModuleName(const std::string &moduleName);
    bool getIsRoot() const;
    std::shared_ptr<ValueType> getValueType() const;

private:
    bool shouldExport;
    std::string identifier;
    std::string moduleName;
    bool isRoot;
    std::shared_ptr<ValueType> valueType;
};

#endif