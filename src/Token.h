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

        EQUAL,
        NOT_EQUAL,
        LESS,
        LESS_EQUAL,
        GREATER,
        GREATER_EQUAL,
        
        LEFT_PAREN,
        RIGHT_PAREN,
        COLON,
        SEMICOLON,

        FUNCTION,
        RETURN,

        INTEGER,
        IDENTIFIER,

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
    static vector<Token::Kind> tokensEquality;
    static vector<Token::Kind> tokensComparison;
    static vector<Token::Kind> tokensTerm;
    static vector<Token::Kind> tokensFactor;
    static vector<Token::Kind> tokensBinary;

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