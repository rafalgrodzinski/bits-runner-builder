#include "ExpressionInvalid.h"

ExpressionInvalid::ExpressionInvalid(shared_ptr<Token> token):
Expression(ExpressionKind::INVALID, ValueType::NONE), token(token) {
}

shared_ptr<Token> ExpressionInvalid::getToken() {
    return token;
}
