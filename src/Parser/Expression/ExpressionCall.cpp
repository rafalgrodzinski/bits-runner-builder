#include "ExpressionCall.h"

ExpressionCall::ExpressionCall(string name, vector<shared_ptr<Expression>> argumentExpressions, int line, int column):
Expression(ExpressionKind::CALL, nullptr, line, column), name(name), argumentExpressions(argumentExpressions) { }

string ExpressionCall::getName() {
    return name;
}

vector<shared_ptr<Expression>> ExpressionCall::getArgumentExpressions() {
    return argumentExpressions;
}
