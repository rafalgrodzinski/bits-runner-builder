#include "Token.h"

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

Token::Token(TokenKind kind, string lexme, int line, int column):
kind(kind), lexme(lexme), line(line), column(column) { }

TokenKind Token::getKind() {
    return kind;
}

string Token::getLexme() {
    return lexme;
}

int Token::getLine() {
    return line;
}

int Token::getColumn() {
    return column;
}

bool Token::isOfKind(vector<TokenKind> kinds) {
    for (TokenKind &kind : kinds) {
        if (kind == this->kind)
            return true;
    }
        
    return false;
}
