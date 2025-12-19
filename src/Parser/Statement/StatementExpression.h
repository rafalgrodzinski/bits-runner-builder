#ifndef STATEMENT_EXPRESSION
#define STATEMENT_EXPRESSION

#include "Parser/Statement/Statement.h"

class Expression;

class StatementExpression: public Statement {
private:
    shared_ptr<Expression> expression;

public:
    StatementExpression(shared_ptr<Expression> expression, shared_ptr<Location> location);
    shared_ptr<Expression> getExpression();
};

#endif