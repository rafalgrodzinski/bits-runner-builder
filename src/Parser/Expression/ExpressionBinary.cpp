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
            expression->valueType = ValueType::BOOL;
            break;
        case TokenKind::XOR:
            expression->operation = ExpressionBinaryOperation::XOR;
            expression->valueType = ValueType::BOOL;
            break;
        case TokenKind::AND:
            expression->operation = ExpressionBinaryOperation::AND;
            expression->valueType = ValueType::BOOL;
            break;
        case TokenKind::BIT_OR:
            expression->operation = ExpressionBinaryOperation::BIT_OR;
            expression->valueType = left->getValueType();
            break;
        case TokenKind::BIT_XOR:
            expression->operation = ExpressionBinaryOperation::BIT_XOR;
            expression->valueType = left->getValueType();
            break;
        case TokenKind::BIT_AND:
            expression->operation = ExpressionBinaryOperation::BIT_AND;
            expression->valueType = left->getValueType();
            break;
        case TokenKind::BIT_SHL:
            expression->operation = ExpressionBinaryOperation::BIT_SHL;
            expression->valueType = left->getValueType();
            break;
        case TokenKind::BIT_SHR:
            expression->operation = ExpressionBinaryOperation::BIT_SHR;
            expression->valueType = left->getValueType();
            break;
        case TokenKind::EQUAL:
            expression->operation = ExpressionBinaryOperation::EQUAL;
            expression->valueType = ValueType::BOOL;
            break;
        case TokenKind::NOT_EQUAL:
            expression->operation = ExpressionBinaryOperation::NOT_EQUAL;
            expression->valueType = ValueType::BOOL;
            break;
        case TokenKind::LESS:
            expression->operation = ExpressionBinaryOperation::LESS;
            expression->valueType = ValueType::BOOL;
            break;
        case TokenKind::LESS_EQUAL:
            expression->operation = ExpressionBinaryOperation::LESS_EQUAL;
            expression->valueType = ValueType::BOOL;
            break;
        case TokenKind::GREATER:
            expression->operation = ExpressionBinaryOperation::GREATER;
            expression->valueType = ValueType::BOOL;
            break;
        case TokenKind::GREATER_EQUAL:
            expression->operation = ExpressionBinaryOperation::GREATER_EQUAL;
            expression->valueType = ValueType::BOOL;
            break;
        case TokenKind::PLUS:
            expression->operation = ExpressionBinaryOperation::ADD;
            expression->valueType = left->getValueType();
            break;
        case TokenKind::MINUS:
            expression->operation = ExpressionBinaryOperation::SUB;
            expression->valueType = left->getValueType();
            break;
        case TokenKind::STAR:
            expression->operation = ExpressionBinaryOperation::MUL;
            expression->valueType = left->getValueType();
            break;
        case TokenKind::SLASH:
            expression->operation = ExpressionBinaryOperation::DIV;
            expression->valueType = left->getValueType();
            break;
        case TokenKind::PERCENT:
            expression->operation = ExpressionBinaryOperation::MOD;
            expression->valueType = left->getValueType();
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
