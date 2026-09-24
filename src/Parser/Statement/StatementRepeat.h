#ifndef STATEMENT_REPEAT
#define STATEMENT_REPEAT

#include "Statement.h"

class Expression;
class StatementBlock;

class StatementRepeat: public Statement {
public:
    StatementRepeat(
        std::shared_ptr<Statement> initStatement,
        std::shared_ptr<Statement> postStatement,
        std::shared_ptr<Expression> preConditionExpression,
        std::shared_ptr<Expression> postConditionExpression,
        std::shared_ptr<StatementBlock> bodyBlockStatement,
        std::shared_ptr<Location> location
    );

    std::shared_ptr<Statement> getInitStatement() const;
    std::shared_ptr<Statement> getPostStatement() const;
    std::shared_ptr<Expression> getPreConditionExpression() const;
    std::shared_ptr<Expression> getPostConditionExpression() const;
    std::shared_ptr<StatementBlock> getBodyBlockStatement() const;

private:
    std::shared_ptr<Statement> initStatement;
    std::shared_ptr<Statement> postStatement;
    std::shared_ptr<Expression> preConditionExpression;
    std::shared_ptr<Expression> postConditionExpression;
    std::shared_ptr<StatementBlock> bodyBlockStatement;
};

#endif