#ifndef LEXER_H
#define LEXER_H

#include <vector>
#include "Token.h"

class Lexer {
private:
    std::string source;
    int currentIndex = 0;
    int currentLine = 0;

    Token nextToken();
    Token matchEnd();
    Token matchNewLine();
    Token matchInvalid();
    Token matchSymbol(char symbol, Token::Kind kind);
    Token matchInteger();

    bool isWhiteSpace(int index);
    bool isNewLine(int index);
    bool isDigit(int index);

public:
    Lexer(std::string source);
    std::vector<Token> getTokens();
};

#endif