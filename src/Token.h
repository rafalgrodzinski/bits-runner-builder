#ifndef TOKEN_H
#define TOKEN_H

#include <iostream>

#include "Types.h"

using namespace std;

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
    bool isValid();
    bool isOfKind(vector<TokenKind> kinds);
    string toString();
};

#endif