#include "Error.h"

shared_ptr<Error> Error::lexerError(int line, int column, string lexme) {
    return make_shared<Error>(
        ErrorKind::LEXER_ERROR,
        line,
        column,
        lexme,
        nullptr,
        optional<TokenKind>(),
        optional<Parsee>(),
        optional<string>()
    );
}

shared_ptr<Error> Error::parserError(shared_ptr<Token> actualToken, optional<TokenKind> expectedTokenKind, optional<Parsee> expectedParsee, optional<string> message) {
    return make_shared<Error>(
        ErrorKind::PARSER_ERROR,
        0,
        0,
        optional<string>(),
        actualToken,
        expectedTokenKind,
        expectedParsee,
        message
    );
}

shared_ptr<Error> Error::builderError(int line, int column, string message) {
    return make_shared<Error>(
        ErrorKind::BUILDER_ERROR,
        line,
        column,
        optional<string>(),
        nullptr,
        optional<TokenKind>(),
        optional<Parsee>(),
        message
    );
}

Error::Error(ErrorKind kind, int line, int column, optional<string> lexme, shared_ptr<Token> actualToken, optional<TokenKind> expectedTokenKind, optional<Parsee> expectedParsee, optional<string> message):
kind(kind), line(line), column(column), lexme(lexme), actualToken(actualToken), expectedTokenKind(expectedTokenKind), expectedParsee(expectedParsee), message(message) { }

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

optional<Parsee> Error::getExpectedParsee() {
    return expectedParsee;
}

optional<string> Error::getMessage() {
    return message;
}