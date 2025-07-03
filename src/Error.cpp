#include "Error.h"

Error::Error(int line, int column, string lexme) :
kind(ErrorKind::LEXER_ERROR), line(line), column(column), lexme(lexme) { }

Error::Error(shared_ptr<Token> actualToken, optional<TokenKind> expectedTokenKind, optional<string> message) :
kind(ErrorKind::PARSER_ERROR), actualToken(actualToken), expectedTokenKind(expectedTokenKind), message(message) { }

ErrorKind Error::getKind() {
    return kind;
}

int Error::getLine() {
    return line;
}

int Error::getColumn() {
    return column;
}

string Error::getLexme() {
    return lexme;
}

shared_ptr<Token> Error::getActualToken() {
    return actualToken;
}

optional<TokenKind> Error::getExpectedTokenKind() {
    return expectedTokenKind;
}

optional<string> Error::getMessage() {
    return message;
}