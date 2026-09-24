#ifndef STATEMENT_VARIABLE_H
#define STATEMENT_VARIABLE_H

#include <format>

#include "Statement.h"
#include "StatementVariableDeclaration.h"

class Expression;
class ValueType;

class StatementVariable: public Statement {
friend class Analyzer;

public:
    StatementVariable(
        bool shouldExport,
        const std::string &identifier,
        std::shared_ptr<ValueType> valueType,
        std::shared_ptr<Expression> expression,
        std::shared_ptr<Location> location
    );

    bool getShouldExport() const;
    std::string getIdentifier() const;
    std::string getGlobalIdentifier() const;
    std::string getModuleName() const;
    void setModuleName(const std::string &moduleName);
    bool getIsRoot() const;
    void setIsRoot(bool isRoot);
    std::shared_ptr<ValueType> getValueType() const;
    std::shared_ptr<Expression> getExpression() const;

    std::shared_ptr<StatementVariableDeclaration> getDeclaration() const;

private:
    bool shouldExport;
    std::string identifier;
    std::string moduleName;
    bool isRoot;
    std::shared_ptr<ValueType> valueType;
    std::shared_ptr<Expression> expression;
};

#endif