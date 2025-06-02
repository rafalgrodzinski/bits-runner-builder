#ifndef LEXER_H
#define LEXER_H

#include <vector>

#include "Token.h"

using namespace std;

class Lexer {
private:
    string source;
    int currentIndex = 0;
    int currentLine = 0;
    int currentColumn = 0;

    shared_ptr<Token> nextToken();
    shared_ptr<Token> match(Token::Kind kind, string lexme, bool needsSeparator);
    shared_ptr<Token> matchInteger();
    shared_ptr<Token> matchIdentifier();
    shared_ptr<Token> matchEnd();
    shared_ptr<Token> matchInvalid();

    bool isWhiteSpace(int index);
    bool isDigit(int index);
    bool isIdentifier(int index);
    bool isSeparator(int index);
    void advanceWithToken(shared_ptr<Token> token);

public:
    Lexer(string source);
    vector<shared_ptr<Token>> getTokens();
};

#endif