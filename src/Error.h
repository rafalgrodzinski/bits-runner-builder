#ifndef ERROR_H
#define ERROR_H

#include <iostream>
#include "Parser/Parsee/Parsee.h"

class Token;
enum class TokenKind;

using namespace std;

enum class ErrorKind {
    LEXER_ERROR,
    PARSER_ERROR,
    BUILDER_ERROR,
    BUILDER_MODULE_ERROR
};

class Error {
private:
    ErrorKind kind;
    optional<int> line;
    optional<int> column;
    optional<string> moduleName;
    optional<string>  lexme;
    shared_ptr<Token> actualToken;
    optional<TokenKind> expectedTokenKind;
    optional<Parsee> expectedParsee;
    optional<string> message;

public:
    static shared_ptr<Error> lexerError(int line, int column, string lexme);
    static shared_ptr<Error> parserError(shared_ptr<Token> actualToken, optional<TokenKind> expectedTokenKind, optional<Parsee> expectedParsee, optional<string> message);
    static shared_ptr<Error> builderError(int line, int column, string message);
    static shared_ptr<Error> builderModuleError(string moduleName, string message);

    Error();

    ErrorKind getKind();
    optional<int> getLine();
    optional<int> getColumn();
    optional<string> getModuleName();
    optional<string> getLexme();
    shared_ptr<Token> getActualToken();
    optional<TokenKind> getExpectedTokenKind();
    optional<Parsee> getExpectedParsee();
    optional<string> getMessage();
};

#endif