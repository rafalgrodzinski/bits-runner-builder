#ifndef STATEMENT_VARIABLE_H
#define STATEMENT_VARIABLE_H

#include "Statement.h"

class Expression;
class ValueType;

class StatementVariable: public Statement {
private:
    bool shouldExport;
    string identifier;
    shared_ptr<ValueType> valueType;
    shared_ptr<Expression> expression;

public:
    StatementVariable(bool shouldExport, string identifier, shared_ptr<ValueType> valueType, shared_ptr<Expression> expression);
    bool getShouldExport();
    string getIdentifier();
    shared_ptr<ValueType> getValueType();
    shared_ptr<Expression> getExpression();
};

#endif