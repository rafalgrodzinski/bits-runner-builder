#ifndef STATEMENT_EXPRESSION
#define STATEMENT_EXPRESSION

#include "Parser/Statement/Statement.h"

class Expression;

class StatementExpression: public Statement {
friend class Analyzer;

public:
    StatementExpression(shared_ptr<Expression> expression, shared_ptr<Location> location);

    shared_ptr<Expression> getExpression() const;

private:
    shared_ptr<Expression> expression;
};

#endif