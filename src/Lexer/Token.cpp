#include "Token.h"

#include "Location.h"

vector<TokenKind> Token::tokensLogicalOrXor = {
    TokenKind::OR,
    TokenKind::XOR
};

vector<TokenKind> Token::tokensLogicalAnd = {
    TokenKind::AND
};

vector<TokenKind> Token::tokensLogicalNot = {
    TokenKind::NOT
};

vector<TokenKind> Token::tokensBitwiseTest = {
    TokenKind::BIT_TEST
};

vector<TokenKind> Token::tokensBitwiseOrXor = {
    TokenKind::BIT_OR,
    TokenKind::BIT_XOR
};

vector<TokenKind> Token::tokensBitwiseAnd = {
    TokenKind::BIT_AND
};

vector<TokenKind> Token::tokensBitwiseShiftLeft = {
    TokenKind::LEFT_ANGLE_BRACKET,
    TokenKind::LEFT_ANGLE_BRACKET
};

vector<TokenKind> Token::tokensBitwiseShiftRight = {
    TokenKind::RIGHT_ANGLE_BRACKET,
    TokenKind::RIGHT_ANGLE_BRACKET
};

vector<TokenKind> Token::tokensBitwiseNot = {
    TokenKind::BIT_NOT
};

vector<TokenKind> Token::tokensEquality = {
    TokenKind::EQUAL,
    TokenKind::NOT_EQUAL
};

vector<TokenKind> Token::tokensComparison = {
    TokenKind::LEFT_ANGLE_BRACKET,
    TokenKind::LESS_EQUAL,
    TokenKind::RIGHT_ANGLE_BRACKET,
    TokenKind::GREATER_EQUAL
};

vector<TokenKind> Token::tokensTerm = {
    TokenKind::PLUS,
    TokenKind::MINUS
};

vector<TokenKind> Token::tokensFactor = {
    TokenKind::STAR,
    TokenKind::SLASH,
    TokenKind::PERCENT
};

vector<TokenKind> Token::tokensUnary = {
    TokenKind::PLUS,
    TokenKind::MINUS
};

vector<TokenKind> Token::tokensLiteral = {
    TokenKind::BOOL,
    TokenKind::FLOAT,
    TokenKind::INTEGER_DEC,
    TokenKind::INTEGER_HEX,
    TokenKind::INTEGER_BIN,
    TokenKind::INTEGER_CHAR,
    TokenKind::STRING
};

Token::Token(TokenKind kind, string lexme, shared_ptr<Location> location):
kind(kind), lexme(lexme), location(location) { }

TokenKind Token::getKind() {
    return kind;
}

string Token::getLexme() {
    return lexme;
}

shared_ptr<Location> Token::getLocation() {
    return location;
}

bool Token::isOfKind(vector<TokenKind> kinds) {
    for (TokenKind &kind : kinds) {
        if (kind == this->kind)
            return true;
    }
        
    return false;
}
