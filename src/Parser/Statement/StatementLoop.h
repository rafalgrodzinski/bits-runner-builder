#include "Statement.h"

class Expression;
class StatementBlock;

class StatementLoop: public Statement {
private:
    shared_ptr<Statement> initStatement;
    shared_ptr<Expression> preConditionExpression;
    shared_ptr<Expression> postConditionExpression;
    shared_ptr<StatementBlock> bodyBlockStatement;

public:
    StatementLoop(shared_ptr<Statement> initStatement, shared_ptr<Expression> preConditionExpression, shared_ptr<Expression> postConditionExpression, shared_ptr<StatementBlock> bodyBlockStatement);
    shared_ptr<Statement> getInitStatement();
    shared_ptr<Expression> getPreConditionExpression();
    shared_ptr<Expression> getPostConditionExpression();
    shared_ptr<StatementBlock> getBodyBlockStatement();
    string toString(int indent) override;
};