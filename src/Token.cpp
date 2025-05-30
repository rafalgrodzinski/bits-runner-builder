#include "Token.h"

Token Token::Invalid = Token(Token::Kind::INVALID, "");

Token::Token(Kind kind, std::string lexme): kind(kind), lexme(lexme) {
}

Token::Kind Token::getKind() {
    return kind;
}

std::string Token::getLexme() {
    return lexme;
}

bool Token::operator==(Token const& other) {
    return kind == other.kind;
}

bool Token::operator!=(Token const& other) {
    return kind != other.kind;
}

bool Token::isOneOf(std::vector<Kind> kinds) {
    for (Kind &kind : kinds) {
        if (kind == this->kind)
            return true;
    }
        
    return false;
}

std::string Token::toString() {
    switch (kind) {
        case PLUS:
            return "PLUS";
        case MINUS:
            return "MINUS";
        case STAR:
            return "STAR";
        case SLASH:
            return "SLASH";
        case PERCENT:
            return "PERCENT";
        case LEFT_PAREN:
            return "LEFT_PARENT";
        case RIGHT_PAREN:
            return "RIGHT_PAREN";
        case DOT:
            return "DOT";
        case COMMA:
            return "COMMA";
        case INTEGER:
            return "INTEGER";
        case NEW_LINE:
            return "NEW_LINE";
        case END:
            return "END";
        case INVALID:
            return "INVALID";
    }
}