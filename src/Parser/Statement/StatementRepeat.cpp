#include "StatementRepeat.h"

#include "Parser/Expression/Expression.h"
#include "Parser/Statement/StatementBlock.h"

StatementRepeat::StatementRepeat(shared_ptr<Statement> initStatement, shared_ptr<Expression> preConditionExpression, shared_ptr<Expression> postConditionExpression, shared_ptr<StatementBlock> bodyBlockStatement):
Statement(StatementKind::REPEAT), initStatement(initStatement), preConditionExpression(preConditionExpression), postConditionExpression(postConditionExpression), bodyBlockStatement(bodyBlockStatement) { }

shared_ptr<Statement> StatementRepeat::getInitStatement() {
    return initStatement;
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

string StatementRepeat::toString(int indent) {
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