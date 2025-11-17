#include "ExpressionBinary.h"

#include "Lexer/Token.h"
#include "Parser/ValueType.h"

ExpressionBinary::ExpressionBinary(int line, int column) :
Expression(ExpressionKind::BINARY, nullptr, line, column) { }

shared_ptr<ExpressionBinary> ExpressionBinary::expression(shared_ptr<Token> token, shared_ptr<Expression> left, shared_ptr<Expression> right) {
    if (left == nullptr || right == nullptr)
        return nullptr;

    shared_ptr<ExpressionBinary> expression = make_shared<ExpressionBinary>(token->getLine(), token->getColumn());
    expression->left = left;
    expression->right = right;

    switch (token->getKind()) {
        case TokenKind::OR:
            expression->operation = ExpressionBinaryOperation::OR;
            break;
        case TokenKind::XOR:
            expression->operation = ExpressionBinaryOperation::XOR;
            break;
        case TokenKind::AND:
            expression->operation = ExpressionBinaryOperation::AND;
            break;
        case TokenKind::BIT_OR:
            expression->operation = ExpressionBinaryOperation::BIT_OR;
            break;
        case TokenKind::BIT_XOR:
            expression->operation = ExpressionBinaryOperation::BIT_XOR;
            break;
        case TokenKind::BIT_AND:
            expression->operation = ExpressionBinaryOperation::BIT_AND;
            break;
        case TokenKind::BIT_SHL:
            expression->operation = ExpressionBinaryOperation::BIT_SHL;
            break;
        case TokenKind::BIT_SHR:
            expression->operation = ExpressionBinaryOperation::BIT_SHR;
            break;
        case TokenKind::EQUAL:
            expression->operation = ExpressionBinaryOperation::EQUAL;
            break;
        case TokenKind::NOT_EQUAL:
            expression->operation = ExpressionBinaryOperation::NOT_EQUAL;
            break;
        case TokenKind::LESS:
            expression->operation = ExpressionBinaryOperation::LESS;
            break;
        case TokenKind::LESS_EQUAL:
            expression->operation = ExpressionBinaryOperation::LESS_EQUAL;
            break;
        case TokenKind::GREATER:
            expression->operation = ExpressionBinaryOperation::GREATER;
            break;
        case TokenKind::GREATER_EQUAL:
            expression->operation = ExpressionBinaryOperation::GREATER_EQUAL;
            break;
        case TokenKind::PLUS:
            expression->operation = ExpressionBinaryOperation::ADD;
            break;
        case TokenKind::MINUS:
            expression->operation = ExpressionBinaryOperation::SUB;
            break;
        case TokenKind::STAR:
            expression->operation = ExpressionBinaryOperation::MUL;
            break;
        case TokenKind::SLASH:
            expression->operation = ExpressionBinaryOperation::DIV;
            break;
        case TokenKind::PERCENT:
            expression->operation = ExpressionBinaryOperation::MOD;
            break;
        default:
            return nullptr;;
    }

    return expression;
}

ExpressionBinaryOperation ExpressionBinary::getOperation() {
    return operation;
}

shared_ptr<Expression> ExpressionBinary::getLeft() {
    return left;
}

shared_ptr<Expression> ExpressionBinary::getRight() {
    return right;
}
