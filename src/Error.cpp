#include "Error.h"

shared_ptr<Error> Error::lexerError(int line, int column, string lexme) {
    shared_ptr<Error> error = make_shared<Error>();
    error->kind = ErrorKind::LEXER_ERROR;
    error->line = line;
    error->column = column;
    error->lexme = lexme;
    return error;
}

shared_ptr<Error> Error::parserError(shared_ptr<Token> actualToken, optional<TokenKind> expectedTokenKind, optional<Parsee> expectedParsee, optional<string> message) {
    shared_ptr<Error> error = make_shared<Error>();
    error->kind = ErrorKind::PARSER_ERROR;
    error->actualToken = actualToken;
    error->expectedTokenKind = expectedTokenKind;
    error->expectedParsee = expectedParsee;
    error->message = message;
    return error;
}

shared_ptr<Error> Error::builderError(int line, int column, string message) {
    shared_ptr<Error> error = make_shared<Error>();
    error->kind = ErrorKind::BUILDER_ERROR;
    error->line = line;
    error->column = column;
    error->message = message;
    return error;
}

shared_ptr<Error> Error::builderModuleError(string moduleName, string message) {
    shared_ptr<Error> error = make_shared<Error>();
    error->kind = ErrorKind::BUILDER_MODULE_ERROR;
    error->moduleName = moduleName;
    error->message = message;
    return error;
}

Error::Error() { }

ErrorKind Error::getKind() {
    return kind;
}

optional<int> Error::getLine() {
    return line;
}

optional<int> Error::getColumn() {
    return column;
}

optional<string> Error::getModuleName() {
    return moduleName;
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