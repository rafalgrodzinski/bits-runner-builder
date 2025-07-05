#include "Error.h"

shared_ptr<Error> Error::builderError(int line, int column, string message) {
    Error e = Error(ErrorKind::BUILDER_ERROR, line, column, {}, nullptr, {}, message);
    return nullptr;
}

Error::Error(int line, int column, string lexme) :
kind(ErrorKind::LEXER_ERROR), line(line), column(column), lexme(lexme) { }

Error::Error(shared_ptr<Token> actualToken, optional<TokenKind> expectedTokenKind, optional<string> message) :
kind(ErrorKind::PARSER_ERROR), actualToken(actualToken), expectedTokenKind(expectedTokenKind), message(message) { }

Error::Error(ErrorKind kind, int line, int column, optional<string> lexme, shared_ptr<Token> actualToken, optional<TokenKind> expectedTokenKind, optional<string> message):
kind(kind), line(line), column(column), lexme(lexme), actualToken(actualToken), expectedTokenKind(expectedTokenKind), message(message) { }

ErrorKind Error::getKind() {
    return kind;
}

int Error::getLine() {
    return line;
}

int Error::getColumn() {
    return column;
}

optional<string> Error::getLexme() {
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