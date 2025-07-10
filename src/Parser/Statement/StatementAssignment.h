#include "Parser/Statement/Statement.h"

class Expression;

class StatementAssignment: public Statement {
private:
    string name;
    shared_ptr<Expression> indexExpression;
    shared_ptr<Expression> expression;

public:
    StatementAssignment(string name, shared_ptr<Expression> indexExpressio, shared_ptr<Expression> expression);
    string getName();
    shared_ptr<Expression> getIndexExpression();
    shared_ptr<Expression> getExpression();
};