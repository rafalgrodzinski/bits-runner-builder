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

vector<TokenKind> Token::tokensBitwiseOrXor = {
    TokenKind::BIT_OR,
    TokenKind::BIT_XOR
};

vector<TokenKind> Token::tokensBitwiseAnd = {
    TokenKind::BIT_AND
};

vector<TokenKind> Token::tokensBitwiseShift = {
    TokenKind::BIT_SHL,
    TokenKind::BIT_SHR
};

vector<TokenKind> Token::tokensBitwiseNot = {
    TokenKind::BIT_NOT
};

vector<TokenKind> Token::tokensEquality = {
    TokenKind::EQUAL,
    TokenKind::NOT_EQUAL
};

vector<TokenKind> Token::tokensComparison = {
    TokenKind::LESS,
    TokenKind::LESS_EQUAL,
    TokenKind::GREATER,
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

int Token::getLine() {
    return location->getLine();
}

int Token::getColumn() {
    return location->getColumn();
}

bool Token::isOfKind(vector<TokenKind> kinds) {
    for (TokenKind &kind : kinds) {
        if (kind == this->kind)
            return true;
    }
        
    return false;
}
