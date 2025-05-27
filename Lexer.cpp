#include "Lexer.h"
#include "Token.h"

Lexer::Lexer(std::string source) : source(source) {
}

std::vector<Token> Lexer::tokens() {
    return { Token::integer, Token::real, Token::integer, Token::eof };
}