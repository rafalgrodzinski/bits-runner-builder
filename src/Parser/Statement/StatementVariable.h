#ifndef STATEMENT_VARIABLE_H
#define STATEMENT_VARIABLE_H

#include "Statement.h"

class Expression;
class ValueType;

class StatementVariable: public Statement {
friend class Analyzer;

private:
    bool shouldExport;
    string identifier;
    shared_ptr<ValueType> valueType;
    shared_ptr<Expression> expression;

public:
    StatementVariable(bool shouldExport, const string &identifier, shared_ptr<ValueType> valueType, shared_ptr<Expression> expression, shared_ptr<Location> location);
    bool getShouldExport() const;
    string getIdentifier() const;
    shared_ptr<ValueType> getValueType() const;
    shared_ptr<Expression> getExpression() const;
};

#endif