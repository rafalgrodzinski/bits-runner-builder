#include "ExpressionUnary.h"

#include "Lexer/Token.h"

ExpressionUnary::ExpressionUnary(shared_ptr<Token> token, shared_ptr<Expression> expression):
Expression(ExpressionKind::UNARY, nullptr), expression(expression) {
    switch (token->getKind()) {
        case TokenKind::PLUS:
            operation = ExpressionUnaryOperation::PLUS;
            break;
        case TokenKind::MINUS:
            operation = ExpressionUnaryOperation::MINUS;
            break;
        default:
            operation = ExpressionUnaryOperation::INVALID;
            break;
    }
}

ExpressionUnaryOperation ExpressionUnary::getOperation() {
    return operation;
}

shared_ptr<Expression> ExpressionUnary::getExpression() {
    return expression;
}