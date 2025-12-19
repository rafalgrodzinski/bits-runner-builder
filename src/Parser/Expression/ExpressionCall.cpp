#include "ExpressionCall.h"

ExpressionCall::ExpressionCall(string name, vector<shared_ptr<Expression>> argumentExpressions, shared_ptr<Location> location):
Expression(ExpressionKind::CALL, nullptr, location), name(name), argumentExpressions(argumentExpressions) { }

string ExpressionCall::getName() {
    return name;
}

vector<shared_ptr<Expression>> ExpressionCall::getArgumentExpressions() {
    return argumentExpressions;
}
