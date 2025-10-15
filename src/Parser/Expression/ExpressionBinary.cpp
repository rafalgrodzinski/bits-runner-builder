#include "ExpressionBinary.h"

#include "Lexer/Token.h"
#include "Parser/ValueType.h"

ExpressionBinary::ExpressionBinary(shared_ptr<Token> token, shared_ptr<Expression> left, shared_ptr<Expression> right):
Expression(ExpressionKind::BINARY, nullptr), operation(ExpressionBinaryOperation::INVALID), left(left), right(right) {
    switch (token->getKind()) {
        case TokenKind::OR:
            operation = ExpressionBinaryOperation::OR;
            valueType = ValueType::BOOL;
            break;
        case TokenKind::XOR:
            operation = ExpressionBinaryOperation::XOR;
            valueType = ValueType::BOOL;
            break;
        case TokenKind::AND:
            operation = ExpressionBinaryOperation::AND;
            valueType = ValueType::BOOL;
            break;
        case TokenKind::BIT_OR:
            operation = ExpressionBinaryOperation::BIT_OR;
            valueType = left->getValueType();
            break;
        case TokenKind::BIT_XOR:
            operation = ExpressionBinaryOperation::BIT_XOR;
            valueType = left->getValueType();
            break;
        case TokenKind::BIT_AND:
            operation = ExpressionBinaryOperation::BIT_AND;
            valueType = left->getValueType();
            break;
        case TokenKind::EQUAL:
            operation = ExpressionBinaryOperation::EQUAL;
            valueType = ValueType::BOOL;
            break;
        case TokenKind::NOT_EQUAL:
            operation = ExpressionBinaryOperation::NOT_EQUAL;
            valueType = ValueType::BOOL;
            break;
        case TokenKind::LESS:
            operation = ExpressionBinaryOperation::LESS;
            valueType = ValueType::BOOL;
            break;
        case TokenKind::LESS_EQUAL:
            operation = ExpressionBinaryOperation::LESS_EQUAL;
            valueType = ValueType::BOOL;
            break;
        case TokenKind::GREATER:
            operation = ExpressionBinaryOperation::GREATER;
            valueType = ValueType::BOOL;
            break;
        case TokenKind::GREATER_EQUAL:
            operation = ExpressionBinaryOperation::GREATER_EQUAL;
            valueType = ValueType::BOOL;
            break;
        case TokenKind::PLUS:
            operation = ExpressionBinaryOperation::ADD;
            valueType = left->getValueType();
            break;
        case TokenKind::MINUS:
            operation = ExpressionBinaryOperation::SUB;
            valueType = left->getValueType();
            break;
        case TokenKind::STAR:
            operation = ExpressionBinaryOperation::MUL;
            valueType = left->getValueType();
            break;
        case TokenKind::SLASH:
            operation = ExpressionBinaryOperation::DIV;
            valueType = left->getValueType();
            break;
        case TokenKind::PERCENT:
            operation = ExpressionBinaryOperation::MOD;
            valueType = left->getValueType();
            break;
        default:
            break;
    }

    if (left->getValueType() != nullptr && right->getValueType() != nullptr) {
        // Types must match
        if (left->getValueType() != right->getValueType())
            valueType = ValueType::NONE;

        // Booleans can only do = or !=
        if (valueType->getKind() == ValueTypeKind::BOOL && (token->getKind() != TokenKind::EQUAL || token->getKind() != TokenKind::NOT_EQUAL))
            valueType = ValueType::NONE;
    }
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
