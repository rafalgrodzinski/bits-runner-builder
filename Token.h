#ifndef TOKEN_H
#define TOKEN_H

#include <iostream>

class Token {
public:
    enum Kind {        
        PLUS,
        MINUS,
        STAR,
        SLASH,
        PERCENT,
        
        LEFT_PAREN,
        RIGHT_PAREN,
        DOT,
        COMMA,

        INTEGER,

        NEW_LINE,

        END,
        INVALID
    };

private:
    Kind kind;
    std::string lexme;

public:
    Token(Kind kind, std::string lexme);
    Kind getKind();
    std::string getLexme();
    bool operator==(Token const& other);
    bool operator!=(Token const& other);
    std::string toString();

    static Token Invalid;
};

#endif