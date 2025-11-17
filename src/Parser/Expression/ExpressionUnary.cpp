#include "ExpressionUnary.h"

#include "Lexer/Token.h"
#include "Parser/ValueType.h"

ExpressionUnary::ExpressionUnary(int line, int column) :
Expression(ExpressionKind::UNARY, nullptr, line, column) { }

shared_ptr<ExpressionUnary> ExpressionUnary::expression(shared_ptr<Token> token, shared_ptr<Expression> subExpression) {
    if (subExpression == nullptr)
        return nullptr;
        
    shared_ptr<ExpressionUnary> expression = make_shared<ExpressionUnary>(token->getLine(), token->getColumn());
    expression->subExpression = subExpression;

    switch (token->getKind()) {
        case TokenKind::NOT:
            expression->operation = ExpressionUnaryOperation::NOT;
            break;
        case TokenKind::BIT_NOT:
            expression->operation = ExpressionUnaryOperation::BIT_NOT;
            break;
        case TokenKind::PLUS:
            expression->operation = ExpressionUnaryOperation::PLUS;
            break;
        case TokenKind::MINUS:
            expression->operation = ExpressionUnaryOperation::MINUS;
            break;
        default:
            return nullptr;
    }

    return expression;
}

ExpressionUnaryOperation ExpressionUnary::getOperation() {
    return operation;
}

shared_ptr<Expression> ExpressionUnary::getSubExpression() {
    return subExpression;
}