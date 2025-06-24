#include "StatementLoop.h"

#include "Parser/Expression/Expression.h"
#include "Parser/Statement/StatementBlock.h"

StatementLoop::StatementLoop(shared_ptr<Statement> initStatement, shared_ptr<Expression> preConditionExpression, shared_ptr<Expression> postConditionExpression, shared_ptr<StatementBlock> bodyBlockStatement):
Statement(StatementKind::LOOP), initStatement(initStatement), preConditionExpression(preConditionExpression), postConditionExpression(postConditionExpression), bodyBlockStatement(bodyBlockStatement) { }

shared_ptr<Statement> StatementLoop::getInitStatement() {
    return initStatement;
}

shared_ptr<Expression> StatementLoop::getPreConditionExpression() {
    return preConditionExpression;
}

shared_ptr<Expression> StatementLoop::getPostConditionExpression() {
    return postConditionExpression;
}

shared_ptr<StatementBlock> StatementLoop::getBodyBlockStatement() {
    return bodyBlockStatement;
}

string StatementLoop::toString(int indent) {
    string value;
    for (int ind=0; ind<indent; ind++)
        value += "  ";
    value += "REP(";
    if (initStatement != nullptr)
        value += initStatement->toString(0), ", ";
    if (preConditionExpression != nullptr)
        value += preConditionExpression->toString(0) + ", ";
    if (postConditionExpression != nullptr)
        value += postConditionExpression->toString(0);
    value += "):\n";
    value += bodyBlockStatement->toString(indent+1);
    return value;
}