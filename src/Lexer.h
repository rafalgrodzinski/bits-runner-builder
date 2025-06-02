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

    Token nextToken();
    Token matchEnd();
    Token matchNewLine();
    Token matchInvalid();
    Token matchSymbol(char symbol, Token::Kind kind);
    Token matchKeyword(string keyword, Token::Kind kind);
    Token matchInteger();
    Token matchIdentifier();

    bool isWhiteSpace(int index);
    bool isNewLine(int index);
    bool isDigit(int index);
    bool isIdentifier(int index);
    bool isSeparator(int index);

public:
    Lexer(string source);
    vector<Token> getTokens();
};

#endif