#include "ExpressionCall.h"

ExpressionCall::ExpressionCall(string name, vector<shared_ptr<Expression>> argumentExpressions):
Expression(ExpressionKind::CALL, ValueType::NONE), name(name), argumentExpressions(argumentExpressions) { }

string ExpressionCall::getName() {
    return name;
}

vector<shared_ptr<Expression>> ExpressionCall::getArgumentExpressions() {
    return argumentExpressions;
}
