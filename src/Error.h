#ifndef ERROR_H
#define ERROR_H

#include <iostream>

class Token;
enum class TokenKind;

using namespace std;

enum class ErrorKind {
    LEXER_ERROR,
    PARSER_ERROR
};

class Error {
private:
    ErrorKind kind;

    int line;
    int column;
    string lexme;

    shared_ptr<Token> actualToken;
    optional<TokenKind> expectedTokenKind;
    optional<string> message;

public:
    Error(int line, int column, string lexme);
    Error(shared_ptr<Token> actualToken, optional<TokenKind> expectedTokenKind, optional<string> message);

    ErrorKind getKind();

    int getLine();
    int getColumn();
    string getLexme();

    shared_ptr<Token> getActualToken();
    optional<TokenKind> getExpectedTokenKind();
    optional<string> getMessage();
};

#endif