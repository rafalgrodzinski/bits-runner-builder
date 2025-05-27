#ifndef LEXER_H
#define LEXER_H

#include <vector>

class Token;

class Lexer {
private:
    std::string source;

public:
    Lexer(std::string source);
    std::vector<Token> tokens();
};

#endif