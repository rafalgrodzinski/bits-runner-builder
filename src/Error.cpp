#include "Error.h"

#include "Parser/ValueType.h"
#include "Lexer/Location.h"

shared_ptr<Error> Error::error(int line, int column, string message) {
    shared_ptr<Error> error = make_shared<Error>();
    error->kind = ErrorKind::MESSAGE;
    error->message = message;
    return error;
}

shared_ptr<Error> Error::lexerError(shared_ptr<Location> location, string lexme) {
    shared_ptr<Error> error = make_shared<Error>();
    error->kind = ErrorKind::LEXER_ERROR;
    error->location = location;
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
    error->message = message;
    return error;
}

shared_ptr<Error> Error::builderFunctionError(string functionName, string message) {
    shared_ptr<Error> error = make_shared<Error>();
    error->kind = ErrorKind::BUILDER_FUNCTION_ERROR;
    error->functionName = functionName;
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

shared_ptr<Location> Error::getLocation() {
    return location;
}

optional<int> Error::getLine() {
    return location->getLine();
}

optional<int> Error::getColumn() {
    return location->getColumn();
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

shared_ptr<ValueType> Error::getActualType() {
    return actualType;
}

shared_ptr<ValueType> Error::getExpectedType() {
    return expectedType;
}

shared_ptr<ValueType> Error::getFirstType() {
    return firstType;
}

shared_ptr<ValueType> Error::getSecondType() {
    return secondType;
}

optional<ExpressionUnaryOperation> Error::getUnaryOperation() {
    return unaryOperation;
}

optional<ExpressionBinaryOperation> Error::getBinaryOperation() {
    return binaryOperation;
}

optional<string> Error::getIdentifier() {
    return identifier;
}

optional<string> Error::getFunctionName() {
    return functionName;
}

optional<string> Error::getModuleName() {
    return moduleName;
}

optional<string> Error::getMessage() {
    return message;
}