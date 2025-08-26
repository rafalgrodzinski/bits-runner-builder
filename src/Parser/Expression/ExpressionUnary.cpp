#include "ExpressionUnary.h"

#include "Lexer/Token.h"
#include "Parser/ValueType.h"

ExpressionUnary::ExpressionUnary(shared_ptr<Token> token, shared_ptr<Expression> expression):
Expression(ExpressionKind::UNARY, nullptr), expression(expression) {
    switch (token->getKind()) {
        case TokenKind::NOT:
            operation = ExpressionUnaryOperation::NOT;
            valueType = ValueType::BOOL;
            break;
        case TokenKind::PLUS:
            operation = ExpressionUnaryOperation::PLUS;
            valueType = expression->getValueType();
            break;
        case TokenKind::MINUS:
            operation = ExpressionUnaryOperation::MINUS;
            valueType = expression->getValueType();
            break;
        default:
            operation = ExpressionUnaryOperation::INVALID;
            valueType = nullptr;
            break;
    }
}

ExpressionUnaryOperation ExpressionUnary::getOperation() {
    return operation;
}

shared_ptr<Expression> ExpressionUnary::getExpression() {
    return expression;
}