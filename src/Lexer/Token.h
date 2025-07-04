#ifndef TOKEN_H
#define TOKEN_H

#include <iostream>

#include "Types.h"

using namespace std;

enum class TokenKind {        
    PLUS,
    MINUS,
    STAR,
    SLASH,
    PERCENT,

    EQUAL,
    NOT_EQUAL,
    LESS,
    LESS_EQUAL,
    GREATER,
    GREATER_EQUAL,
    
    LEFT_PAREN,
    RIGHT_PAREN,
    COMMA,
    COLON,
    SEMICOLON,
    LEFT_ARROW,
    RIGHT_ARROW,

    FUNCTION,
    RETURN,
    REPEAT,
    IF,
    ELSE,

    BOOL,
    INTEGER_DEC,
    INTEGER_HEX,
    INTEGER_BIN,
    INTEGER_CHAR,
    REAL,
    IDENTIFIER,
    TYPE,

    M_EXTERN,

    NEW_LINE,
    END
};

class Token {
private:
    TokenKind kind;
    string lexme;
    int line;
    int column;

public:
    static vector<TokenKind> tokensEquality;
    static vector<TokenKind> tokensComparison;
    static vector<TokenKind> tokensTerm;
    static vector<TokenKind> tokensFactor;
    static vector<TokenKind> tokensBinary;
    static vector<TokenKind> tokensLiteral;

    Token(TokenKind kind, string lexme, int line, int column);
    TokenKind getKind();
    string getLexme();
    int getLine();
    int getColumn();
    bool isOfKind(vector<TokenKind> kinds);
};

#endif