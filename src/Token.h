#ifndef TOKEN_H
#define TOKEN_H

#include <iostream>

using namespace std;

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

        FUNCTION,

        NEW_LINE,
        END,

        INVALID
    };

private:
    Kind kind;
    string lexme;
    int line;
    int column;

public:
    Token(Kind kind, string lexme, int line, int column);
    Kind getKind();
    string getLexme();
    int getLine();
    int getColumn();
    bool isValid();
    bool isOfKind(vector<Kind> kinds);
    string toString();
};

#endif