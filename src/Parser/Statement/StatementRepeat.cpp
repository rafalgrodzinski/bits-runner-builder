#include "StatementRepeat.h"

#include "Parser/Expression/Expression.h"
#include "Parser/Statement/StatementBlock.h"

StatementRepeat::StatementRepeat(
    shared_ptr<Statement> initStatement,
    shared_ptr<Statement> postStatement,
    shared_ptr<Expression> preConditionExpression,
    shared_ptr<Expression> postConditionExpression,
    shared_ptr<StatementBlock> bodyBlockStatement
):
Statement(StatementKind::REPEAT), initStatement(initStatement), postStatement(postStatement), preConditionExpression(preConditionExpression), postConditionExpression(postConditionExpression), bodyBlockStatement(bodyBlockStatement) { }

shared_ptr<Statement> StatementRepeat::getInitStatement() {
    return initStatement;
}

shared_ptr<Statement> StatementRepeat::getPostStatement() {
    return postStatement;
}

shared_ptr<Expression> StatementRepeat::getPreConditionExpression() {
    return preConditionExpression;
}

shared_ptr<Expression> StatementRepeat::getPostConditionExpression() {
    return postConditionExpression;
}

shared_ptr<StatementBlock> StatementRepeat::getBodyBlockStatement() {
    return bodyBlockStatement;
}
