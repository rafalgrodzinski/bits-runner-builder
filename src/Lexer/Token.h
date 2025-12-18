#ifndef TOKEN_H
#define TOKEN_H

#include <iostream>
#include <vector>

class Location;

using namespace std;

enum class TokenKind {        
    OR,
    XOR,
    AND,
    NOT,

    BIT_OR,
    BIT_XOR,
    BIT_AND,
    BIT_NOT,
    BIT_SHL,
    BIT_SHR,

    EQUAL,
    NOT_EQUAL,
    LESS,
    LESS_EQUAL,
    GREATER,
    GREATER_EQUAL,

    PLUS,
    MINUS,
    STAR,
    SLASH,
    PERCENT,
    
    LEFT_ROUND_BRACKET,
    RIGHT_ROUND_BRACKET,
    LEFT_SQUARE_BRACKET,
    RIGHT_SQUARE_BRACKET,
    LEFT_CURLY_BRACKET,
    RIGHT_CURLY_BRACKET,
    COMMA,
    COLON,
    SEMICOLON,
    LEFT_ARROW,
    RIGHT_ARROW,
    DOT,

    FUNCTION,
    RAW_FUNCTION,
    RAW_SOURCE_LINE,
    DATA,
    BLOB,
    PTR,
    RETURN,
    REPEAT,
    IF,
    ELSE,

    BOOL,
    FLOAT,
    INTEGER_DEC,
    INTEGER_HEX,
    INTEGER_BIN,
    INTEGER_CHAR,
    STRING,
    TYPE,
    IDENTIFIER,

    M_MODULE,
    M_IMPORT,
    M_EXPORT,
    M_EXTERN,
    META,

    NEW_LINE,
    END
};

class Token {
private:
    TokenKind kind;
    string lexme;
    shared_ptr<Location> location;

public:
    static vector<TokenKind> tokensLogicalOrXor;
    static vector<TokenKind> tokensLogicalAnd;
    static vector<TokenKind> tokensLogicalNot;

    static vector<TokenKind> tokensEquality;
    static vector<TokenKind> tokensComparison;

    static vector<TokenKind> tokensBitwiseAnd;
    static vector<TokenKind> tokensBitwiseOrXor;
    static vector<TokenKind> tokensBitwiseShift;
    static vector<TokenKind> tokensBitwiseNot;

    static vector<TokenKind> tokensTerm;
    static vector<TokenKind> tokensFactor;
    static vector<TokenKind> tokensUnary;

    static vector<TokenKind> tokensLiteral;

    Token(TokenKind kind, string lexme, shared_ptr<Location> location);
    TokenKind getKind();
    string getLexme();
    shared_ptr<Location> getLocation();
    bool isOfKind(vector<TokenKind> kinds);
};

#endif