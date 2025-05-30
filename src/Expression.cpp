#include "Expression.h"

std::shared_ptr<Expression> Expression::Invalid = std::make_shared<Expression>(Expression::Kind::INVALID, Token::Invalid, nullptr, nullptr);

Expression::Expression(Kind kind, Token token, shared_ptr<Expression> left, shared_ptr<Expression> right) {
    switch (kind) {
        case LITERAL:
            setupLiteral(token);
            break;
        case BINARY:
            setupBinary(token, left, right);
            break;
        case INVALID:
            break;
    }
}

void Expression::setupLiteral(Token token) {
    bool isKindValid = token.isOneOf({Token::Kind::INTEGER});
    if (!isKindValid)
        return;

    kind = LITERAL;
    integer = stoi(token.getLexme());
}

void Expression::setupBinary(Token token, shared_ptr<Expression> left, shared_ptr<Expression> right) {
    bool isKindValid = token.isOneOf({Token::Kind::PLUS, Token::Kind::MINUS, Token::Kind::STAR, Token::Kind::SLASH, Token::Kind::PERCENT});
    bool isLeftValid = left != nullptr && left->getKind() != Kind::INVALID;
    bool isRightValid = right != nullptr && right->getKind() != Kind::INVALID;

    if (!isKindValid || !isLeftValid || !isRightValid)
        return;

    kind = BINARY;

    switch (token.getKind()) {
        case Token::Kind::PLUS:
            operation = ADD;
            break;
        case Token::Kind::MINUS:
            operation = SUB;
            break;
        case Token::Kind::STAR:
            operation = MUL;
            break;
        case Token::Kind::SLASH:
            operation = DIV;
            break;
        case Token::Kind::PERCENT:
            operation = MOD;
            break;
        case Token::Kind::INVALID:
            break;
    }

    this->left = left;
    this->right = right;
}

Expression::Kind Expression::getKind() {
    return kind;
}

int64_t Expression::getInteger() {
    return integer;
}

Expression::Operator Expression::getOperator() {
    return operation;
}

shared_ptr<Expression> Expression::getLeft() {
    return left;
}

shared_ptr<Expression> Expression::getRight() {
    return right;
}

bool Expression::operator==(Expression const& other) {
    return kind == other.kind;
}

bool Expression::operator!=(Expression const& other) {
    return kind != other.kind;
}

std::string Expression::toString() {
    switch (kind) {
        case LITERAL:
            return to_string(integer);
        case BINARY:
            switch (operation) {
                case ADD:
                    return "<+ " + left->toString() + " " + right->toString() + ">";
                case SUB:
                    return "<- " + left->toString() + " " + right->toString() + ">";
                case MUL:
                    return "<* " + left->toString() + " " + right->toString() + ">";
                case DIV:
                    return "</ " + left->toString() + " " + right->toString() + ">";
                case MOD:
                    return "<% " + left->toString() + " " + right->toString() + ">";
                case NONE:
                    return "NONE";
            }
        case INVALID:
            return "INVALID";
    }
}