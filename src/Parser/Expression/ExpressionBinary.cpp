#include "ExpressionBinary.h"

#include "Lexer/Token.h"
#include "Parser/ValueType.h"

ExpressionBinary::ExpressionBinary(shared_ptr<Location> location) :
Expression(ExpressionKind::BINARY, nullptr, location) { }

shared_ptr<ExpressionBinary> ExpressionBinary::expression(vector<shared_ptr<Token>> tokens, shared_ptr<Expression> left, shared_ptr<Expression> right) {
    if (left == nullptr || right == nullptr)
        return nullptr;

    shared_ptr<ExpressionBinary> expression = make_shared<ExpressionBinary>(tokens.front()->getLocation());
    expression->left = left;
    expression->right = right;

    if (doTokensMatchTokenKinds(tokens, {TokenKind::OR})) {
        expression->operation = ExpressionBinaryOperation::OR;
    } else if (doTokensMatchTokenKinds(tokens, {TokenKind::XOR})) {
        expression->operation = ExpressionBinaryOperation::XOR;
    } else if (doTokensMatchTokenKinds(tokens, {TokenKind::AND})) {
        expression->operation = ExpressionBinaryOperation::AND;
    } else if (doTokensMatchTokenKinds(tokens, {TokenKind::BIT_TEST})) {
        expression->operation = ExpressionBinaryOperation::BIT_TEST;
    } else if (doTokensMatchTokenKinds(tokens, {TokenKind::BIT_OR})) {
        expression->operation = ExpressionBinaryOperation::BIT_OR;
    } else if (doTokensMatchTokenKinds(tokens, {TokenKind::BIT_XOR})) {
        expression->operation = ExpressionBinaryOperation::BIT_XOR;
    } else if (doTokensMatchTokenKinds(tokens, {TokenKind::BIT_AND})) {
        expression->operation = ExpressionBinaryOperation::BIT_AND;
    } else if (doTokensMatchTokenKinds(tokens, {TokenKind::LEFT_ANGLE_BRACKET, TokenKind::LEFT_ANGLE_BRACKET})) {
        expression->operation = ExpressionBinaryOperation::BIT_SHL;
    } else if (doTokensMatchTokenKinds(tokens, {TokenKind::RIGHT_ANGLE_BRACKET, TokenKind::RIGHT_ANGLE_BRACKET})) {
        expression->operation = ExpressionBinaryOperation::BIT_SHR;
    } else if (doTokensMatchTokenKinds(tokens, {TokenKind::EQUAL})) {
        expression->operation = ExpressionBinaryOperation::EQUAL;
    } else if (doTokensMatchTokenKinds(tokens, {TokenKind::NOT_EQUAL})) {
        expression->operation = ExpressionBinaryOperation::NOT_EQUAL;
    } else if (doTokensMatchTokenKinds(tokens, {TokenKind::LEFT_ANGLE_BRACKET})) {
        expression->operation = ExpressionBinaryOperation::LESS;
    } else if (doTokensMatchTokenKinds(tokens, {TokenKind::LESS_EQUAL})) {
        expression->operation = ExpressionBinaryOperation::LESS_EQUAL;
    } else if (doTokensMatchTokenKinds(tokens, {TokenKind::RIGHT_ANGLE_BRACKET})) {
        expression->operation = ExpressionBinaryOperation::GREATER;
    } else if (doTokensMatchTokenKinds(tokens, {TokenKind::GREATER_EQUAL})) {
        expression->operation = ExpressionBinaryOperation::GREATER_EQUAL;
    } else if (doTokensMatchTokenKinds(tokens, {TokenKind::PLUS})) {
        expression->operation = ExpressionBinaryOperation::ADD;
    } else if (doTokensMatchTokenKinds(tokens, {TokenKind::MINUS})) {
        expression->operation = ExpressionBinaryOperation::SUB;
    } else if (doTokensMatchTokenKinds(tokens, {TokenKind::STAR})) {
        expression->operation = ExpressionBinaryOperation::MUL;
    } else if (doTokensMatchTokenKinds(tokens, {TokenKind::SLASH})) {
        expression->operation = ExpressionBinaryOperation::DIV;
    } else if (doTokensMatchTokenKinds(tokens, {TokenKind::PERCENT})) {
        expression->operation = ExpressionBinaryOperation::MOD;
    } else {
        return nullptr;
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

bool ExpressionBinary::doTokensMatchTokenKinds(vector<shared_ptr<Token>> tokens, vector<TokenKind> tokenKinds) {
    // check if not empty and if sizes match
    if (tokens.empty() || tokens.size() != tokenKinds.size())
        return false;

    // then check each kind
    for (int i=0; i<tokens.size(); i++) {
        if (tokens.at(i)->getKind() != tokenKinds.at(i))
            return false;
    }

    return true;
}
