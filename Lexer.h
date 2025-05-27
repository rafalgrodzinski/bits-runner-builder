#ifndef LEXER_H
#define LEXER_H

#include <vector>

class Token;

class Lexer {
public:
    std::vector<Token> tokens();
};

#endif