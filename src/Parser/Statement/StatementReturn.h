#ifndef STATEMENT_RETURN
#define STATEMENT_RETURN

#include "Parser/Statement/Statement.h"

class Expression;

class StatementReturn: public Statement {
friend class TypesAnalyzer;

private:
    shared_ptr<Expression> expression;

public:
    StatementReturn(shared_ptr<Expression> expression, int line, int column);
    shared_ptr<Expression> getExpression();
};

#endif