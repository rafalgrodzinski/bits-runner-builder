#include "ExpressionCall.h"

ExpressionCall::ExpressionCall(string name, vector<shared_ptr<Expression>> argumentExpressions, shared_ptr<Location> location):
Expression(ExpressionKind::CALL, nullptr, location), name(std::move(name)), argumentExpressions(std::move(argumentExpressions)) { }

string ExpressionCall::getName() const {
    return name;
}

vector<shared_ptr<Expression>> ExpressionCall::getArgumentExpressions() const {
    return argumentExpressions;
}
