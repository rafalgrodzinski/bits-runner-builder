#ifndef STATEMENT_VARIABLE_H
#define STATEMENT_VARIABLE_H

#include "Statement.h"
#include "StatementVariableDeclaration.h"

class Expression;
class ValueType;

class StatementVariable: public Statement {
friend class Analyzer;

private:
    bool isRoot;
    bool shouldExport;
    string identifier;
    string moduleName;
    shared_ptr<ValueType> valueType;
    shared_ptr<Expression> expression;

public:
    StatementVariable(bool isRoot, bool shouldExport, const string &identifier, shared_ptr<ValueType> valueType, shared_ptr<Expression> expression, shared_ptr<Location> location);

    bool getIsRoot() const;
    bool getShouldExport() const;
    string getIdentifier() const;
    string getGlobalIdentifier() const;
    string getModuleName() const;
    void setModuleName(const string &moduleName);
    shared_ptr<ValueType> getValueType() const;
    shared_ptr<Expression> getExpression() const;

    shared_ptr<StatementVariableDeclaration> getDeclaration() const;
};

#endif