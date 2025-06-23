#include "Parser/Statement/Statement.h"

class StatementExpression: public Statement {
private:
    shared_ptr<Expression> expression;

public:
    StatementExpression(shared_ptr<Expression> expression);
    shared_ptr<Expression> getExpression();
    string toString(int indent) override;
};