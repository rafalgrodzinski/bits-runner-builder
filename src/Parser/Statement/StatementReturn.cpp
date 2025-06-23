#include "Parser/Statement/StatementReturn.h"

#include "Parser/Expression.h"

StatementReturn::StatementReturn(shared_ptr<Expression> expression):
Statement(StatementKind::RETURN), expression(expression) { }

shared_ptr<Expression> StatementReturn::getExpression() {
    return expression;
}

string StatementReturn::toString(int indent) {
    string value;
    for (int ind=0; ind<indent; ind++)
        value += "  ";
    value += "RETURN";
    if (expression != nullptr) {
        value += ":\n";
        for (int ind=0; ind<indent+1; ind++)
            value += "  ";
        value += expression->toString(indent+1);
    }
    value += "\n";
    return value;
}