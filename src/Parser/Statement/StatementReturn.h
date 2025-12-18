#ifndef STATEMENT_RETURN
#define STATEMENT_RETURN

#include "Parser/Statement/Statement.h"

class Expression;

class StatementReturn: public Statement {
friend class Analyzer;

private:
    shared_ptr<Expression> expression;

public:
    StatementReturn(shared_ptr<Expression> expression, shared_ptr<Location> location);
    shared_ptr<Expression> getExpression();
};

#endif