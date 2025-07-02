#include "Parser/Statement/Statement.h"

class Expression;

class StatementReturn: public Statement {
private:
    shared_ptr<Expression> expression;

public:
    StatementReturn(shared_ptr<Expression> expression);
    shared_ptr<Expression> getExpression();
};