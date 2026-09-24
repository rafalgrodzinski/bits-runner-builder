#ifndef TOKEN_H
#define TOKEN_H

#include <iostream>
#include <memory>
#include <vector>

class Location;

enum class TokenKind {        
    OR,
    XOR,
    AND,
    NOT,

    BIT_TEST,
    BIT_OR,
    BIT_XOR,
    BIT_AND,
    BIT_NOT,

    EQUAL,
    NOT_EQUAL,

    LESS_EQUAL,
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
    LEFT_ANGLE_BRACKET,
    RIGHT_ANGLE_BRACKET,
    COMMA,
    COLON,
    DOUBLE_COLON,
    SEMICOLON,
    LEFT_ARROW,
    RIGHT_ARROW,
    DOT,

    FUNCTION,
    RAW_FUNCTION,
    RAW_SOURCE_LINE,
    DATA,
    BLOB,
    BLOB_PACK,
    PROTO,
    BOXED,
    PTR,
    PTR_VOLATILE,
    RETURN,
    REPEAT,
    IF,
    ELSE,
    ENUM,

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
public:
    static std::vector<TokenKind> tokensLogicalOrXor;
    static std::vector<TokenKind> tokensLogicalAnd;
    static std::vector<TokenKind> tokensLogicalNot;

    static std::vector<TokenKind> tokensEquality;
    static std::vector<TokenKind> tokensComparison;

    static std::vector<TokenKind> tokensBitwiseTest;
    static std::vector<TokenKind> tokensBitwiseOrXor;
    static std::vector<TokenKind> tokensBitwiseAnd;
    static std::vector<TokenKind> tokensBitwiseShiftLeft;
    static std::vector<TokenKind> tokensBitwiseShiftRight;
    static std::vector<TokenKind> tokensBitwiseNot;

    static std::vector<TokenKind> tokensTerm;
    static std::vector<TokenKind> tokensFactor;
    static std::vector<TokenKind> tokensUnary;

    static std::vector<TokenKind> tokensLiteral;

    Token(TokenKind kind, const std::string &lexme, std::shared_ptr<Location> location);

    TokenKind getKind() const;
    std::string getLexme() const;
    std::shared_ptr<Location> getLocation() const;
    bool isOfKind(std::vector<TokenKind> kinds) const;

private:
    TokenKind kind;
    std::string lexme;
    std::shared_ptr<Location> location;
};

#endif