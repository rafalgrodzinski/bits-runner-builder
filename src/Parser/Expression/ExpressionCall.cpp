#include "ExpressionCall.h"

ExpressionCall::ExpressionCall(string name, vector<shared_ptr<Expression>> argumentExpressions):
Expression(ExpressionKind::CALL, ValueType::NONE), name(name), argumentExpressions(argumentExpressions) { }

string ExpressionCall::getName() {
    return name;
}

vector<shared_ptr<Expression>> ExpressionCall::getArgumentExpressions() {
    return argumentExpressions;
}

string ExpressionCall::toString(int indent) {
    string value;

    value += "CALL(" + name + "):";
    for (shared_ptr<Expression> &argumentExpression : argumentExpressions) {
        value += "\n";
        for (int ind=0; ind<indent+1; ind++)
            value += "  ";
        value += argumentExpression->toString(indent+1) + ",";
    }

    return value;
}