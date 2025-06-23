#include "StatementExpression.h"

#include "Parser/Expression.h"

StatementExpression::StatementExpression(shared_ptr<Expression> expression):
Statement(StatementKind::EXPRESSION), expression(expression) { }

shared_ptr<Expression> StatementExpression::getExpression() {
    return expression;
}

string StatementExpression::toString(int indent) {
    string value;
    for (int ind=0; ind<indent; ind++)
        value += "  ";
    value += expression->toString(indent);
    value += "\n";
    return value;
}