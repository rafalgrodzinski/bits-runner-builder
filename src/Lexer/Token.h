#ifndef TOKEN_H
#define TOKEN_H

#include <iostream>

using namespace std;

enum class TokenKind {        
    PLUS,
    MINUS,
    STAR,
    SLASH,
    PERCENT,

    OR,
    XOR,
    AND,
    NOT,

    BIT_OR,
    BIT_XOR,
    BIT_AND,
    BIT_NOT,
    SHL,
    SHR,

    EQUAL,
    NOT_EQUAL,
    LESS,
    LESS_EQUAL,
    GREATER,
    GREATER_EQUAL,
    
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
    BLOB,
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
    int line;
    int column;

public:
    static vector<TokenKind> tokensLogicalSecond;
    static vector<TokenKind> tokensLogicalFirst;
    static vector<TokenKind> tokensLogicalUnary;
    static vector<TokenKind> tokensEquality;
    static vector<TokenKind> tokensComparison;
    static vector<TokenKind> tokensBitwiseAnd;
    static vector<TokenKind> tokensBitwiseOrXor;
    static vector<TokenKind> tokensBitwiseNot;
    static vector<TokenKind> tokensTerm;
    static vector<TokenKind> tokensFactor;
    static vector<TokenKind> tokensUnary;
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