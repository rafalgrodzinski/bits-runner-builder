#include "Error.h"

#include "Parser/ValueType/ValueType.h"
#include "Lexer/Location.h"

shared_ptr<Error> Error::error(shared_ptr<Location> location, const string &message) {
    shared_ptr<Error> error = make_shared<Error>();
    error->kind = ErrorKind::MESSAGE;
    error->location = location;
    error->message = message;
    return error;
}

shared_ptr<Error> Error::lexerError(shared_ptr<Location> location, const string &lexme) {
    shared_ptr<Error> error = make_shared<Error>();
    error->kind = ErrorKind::LEXER_ERROR;
    error->location = location;
    error->lexme = lexme;
    return error;
}

shared_ptr<Error> Error::parserError(shared_ptr<Token> actualToken, optional<TokenKind> expectedTokenKind, const optional<Parsee> &expectedParsee, const optional<string> &message) {
    shared_ptr<Error> error = make_shared<Error>();
    error->kind = ErrorKind::PARSER_ERROR;
    error->actualToken = actualToken;
    error->expectedTokenKind = expectedTokenKind;
    error->expectedParsee = expectedParsee;
    error->message = message;
    return error;
}

shared_ptr<Error> Error::builderFunctionError(const string &functionName, const string &message) {
    shared_ptr<Error> error = make_shared<Error>();
    error->kind = ErrorKind::BUILDER_FUNCTION_ERROR;
    error->functionName = functionName;
    error->message = message;
    return error;
}

shared_ptr<Error> Error::builderModuleError(const string &moduleName, const string &message) {
    shared_ptr<Error> error = make_shared<Error>();
    error->kind = ErrorKind::BUILDER_MODULE_ERROR;
    error->moduleName = moduleName;
    error->message = message;
    return error;
}

Error::Error() { }

ErrorKind Error::getKind() const {
    return kind;
}

shared_ptr<Location> Error::getLocation() const {
    return location;
}

optional<string> Error::getLexme() const {
    return lexme;
}

shared_ptr<Token> Error::getActualToken() const {
    return actualToken;
}

optional<TokenKind> Error::getExpectedTokenKind() const {
    return expectedTokenKind;
}

optional<Parsee> Error::getExpectedParsee() const {
    return expectedParsee;
}

shared_ptr<ValueType> Error::getActualType() const {
    return actualType;
}

shared_ptr<ValueType> Error::getExpectedType() const {
    return expectedType;
}

shared_ptr<ValueType> Error::getFirstType() const {
    return firstType;
}

shared_ptr<ValueType> Error::getSecondType() const {
    return secondType;
}

optional<ExpressionUnaryOperation> Error::getUnaryOperation() const {
    return unaryOperation;
}

optional<ExpressionBinaryOperation> Error::getBinaryOperation() const {
    return binaryOperation;
}

optional<string> Error::getIdentifier() const {
    return identifier;
}

optional<string> Error::getFunctionName() const {
    return functionName;
}

optional<string> Error::getModuleName() const {
    return moduleName;
}

optional<string> Error::getMessage() const {
    return message;
}