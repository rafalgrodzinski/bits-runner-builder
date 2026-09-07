#include "ExpressionBinary.h"

#include "Lexer/Token.h"
#include "Parser/ValueType/ValueType.h"

ExpressionBinary::ExpressionBinary(shared_ptr<Location> location):
Expression(ExpressionKind::BINARY, nullptr, location) { }

shared_ptr<ExpressionBinary> ExpressionBinary::expression(const vector<shared_ptr<Token>> &tokens, shared_ptr<Expression> left, shared_ptr<Expression> right) {
    ExpressionBinaryOperation operation;

    if (doTokensMatchTokenKinds(tokens, {TokenKind::OR})) {
        operation = ExpressionBinaryOperation::OR;
    } else if (doTokensMatchTokenKinds(tokens, {TokenKind::XOR})) {
        operation = ExpressionBinaryOperation::XOR;
    } else if (doTokensMatchTokenKinds(tokens, {TokenKind::AND})) {
        operation = ExpressionBinaryOperation::AND;
    } else if (doTokensMatchTokenKinds(tokens, {TokenKind::BIT_TEST})) {
        operation = ExpressionBinaryOperation::BIT_TEST;
    } else if (doTokensMatchTokenKinds(tokens, {TokenKind::BIT_OR})) {
        operation = ExpressionBinaryOperation::BIT_OR;
    } else if (doTokensMatchTokenKinds(tokens, {TokenKind::BIT_XOR})) {
        operation = ExpressionBinaryOperation::BIT_XOR;
    } else if (doTokensMatchTokenKinds(tokens, {TokenKind::BIT_AND})) {
        operation = ExpressionBinaryOperation::BIT_AND;
    } else if (doTokensMatchTokenKinds(tokens, {TokenKind::LEFT_ANGLE_BRACKET, TokenKind::LEFT_ANGLE_BRACKET})) {
        operation = ExpressionBinaryOperation::BIT_SHL;
    } else if (doTokensMatchTokenKinds(tokens, {TokenKind::RIGHT_ANGLE_BRACKET, TokenKind::RIGHT_ANGLE_BRACKET})) {
        operation = ExpressionBinaryOperation::BIT_SHR;
    } else if (doTokensMatchTokenKinds(tokens, {TokenKind::EQUAL})) {
        operation = ExpressionBinaryOperation::EQUAL;
    } else if (doTokensMatchTokenKinds(tokens, {TokenKind::NOT_EQUAL})) {
        operation = ExpressionBinaryOperation::NOT_EQUAL;
    } else if (doTokensMatchTokenKinds(tokens, {TokenKind::LEFT_ANGLE_BRACKET})) {
        operation = ExpressionBinaryOperation::LESS;
    } else if (doTokensMatchTokenKinds(tokens, {TokenKind::LESS_EQUAL})) {
        operation = ExpressionBinaryOperation::LESS_EQUAL;
    } else if (doTokensMatchTokenKinds(tokens, {TokenKind::RIGHT_ANGLE_BRACKET})) {
        operation = ExpressionBinaryOperation::GREATER;
    } else if (doTokensMatchTokenKinds(tokens, {TokenKind::GREATER_EQUAL})) {
        operation = ExpressionBinaryOperation::GREATER_EQUAL;
    } else if (doTokensMatchTokenKinds(tokens, {TokenKind::PLUS})) {
        operation = ExpressionBinaryOperation::ADD;
    } else if (doTokensMatchTokenKinds(tokens, {TokenKind::MINUS})) {
        operation = ExpressionBinaryOperation::SUB;
    } else if (doTokensMatchTokenKinds(tokens, {TokenKind::STAR})) {
        operation = ExpressionBinaryOperation::MUL;
    } else if (doTokensMatchTokenKinds(tokens, {TokenKind::SLASH})) {
        operation = ExpressionBinaryOperation::DIV;
    } else if (doTokensMatchTokenKinds(tokens, {TokenKind::PERCENT})) {
        operation = ExpressionBinaryOperation::MOD;
    } else {
        return nullptr;
    }

    return ExpressionBinary::expression(operation, left, right, tokens.front()->getLocation());
}

shared_ptr<ExpressionBinary> ExpressionBinary::expression(ExpressionBinaryOperation operation, shared_ptr<Expression> left, shared_ptr<Expression> right, shared_ptr<Location> location) {
    if (left == nullptr || right == nullptr)
        return nullptr;

    shared_ptr<ExpressionBinary> expression = make_shared<ExpressionBinary>(location);
    expression->operation = operation;
    expression->left = left;
    expression->right = right;

    return expression;
}

ExpressionBinaryOperation ExpressionBinary::getOperation() const {
    return operation;
}

shared_ptr<Expression> ExpressionBinary::getLeft() const {
    return left;
}

shared_ptr<Expression> ExpressionBinary::getRight() const {
    return right;
}

bool ExpressionBinary::doTokensMatchTokenKinds(const vector<shared_ptr<Token>> &tokens, const vector<TokenKind> &tokenKinds) {
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
