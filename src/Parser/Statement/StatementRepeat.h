#ifndef STATEMENT_REPEAT
#define STATEMENT_REPEAT

#include "Statement.h"

class Expression;
class StatementBlock;

class StatementRepeat: public Statement {
private:
    shared_ptr<Statement> initStatement;
    shared_ptr<Statement> postStatement;
    shared_ptr<Expression> preConditionExpression;
    shared_ptr<Expression> postConditionExpression;
    shared_ptr<StatementBlock> bodyBlockStatement;

public:
    StatementRepeat(
        shared_ptr<Statement> initStatement,
        shared_ptr<Statement> postStatement,
        shared_ptr<Expression> preConditionExpression,
        shared_ptr<Expression> postConditionExpression,
        shared_ptr<StatementBlock> bodyBlockStatement,
        shared_ptr<Location> location
    );

    shared_ptr<Statement> getInitStatement() const;
    shared_ptr<Statement> getPostStatement() const;
    shared_ptr<Expression> getPreConditionExpression() const;
    shared_ptr<Expression> getPostConditionExpression() const;
    shared_ptr<StatementBlock> getBodyBlockStatement() const;
};

#endif