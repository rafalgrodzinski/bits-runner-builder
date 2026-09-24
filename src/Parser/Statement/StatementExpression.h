#ifndef STATEMENT_EXPRESSION
#define STATEMENT_EXPRESSION

#include "Parser/Statement/Statement.h"

class Expression;

class StatementExpression: public Statement {
friend class Analyzer;

public:
    StatementExpression(std::shared_ptr<Expression> expression, std::shared_ptr<Location> location);

    std::shared_ptr<Expression> getExpression() const;

private:
    std::shared_ptr<Expression> expression;
};

#endif