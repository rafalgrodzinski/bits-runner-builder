#include "ExpressionInvalid.h"

ExpressionInvalid::ExpressionInvalid(shared_ptr<Token> token):
Expression(ExpressionKind::INVALID, ValueType::NONE), token(token) {
}

shared_ptr<Token> ExpressionInvalid::getToken() {
    return token;
}

string ExpressionInvalid::toString(int indent) {
    return "Invalid token " + token->toString() + " at " + to_string(token->getLine()) + ":" + to_string(token->getColumn()) + "\n";
}