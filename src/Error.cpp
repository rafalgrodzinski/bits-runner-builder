#include "Error.h"

#include "Parser/ValueType/ValueType.h"
#include "Lexer/Location.h"

shared_ptr<Error> Error::error(shared_ptr<Location> location, string message) {
    shared_ptr<Error> error = make_shared<Error>();
    error->kind = ErrorKind::MESSAGE;
    error->location = location;
    error->message = std::move(message);
    return error;
}

shared_ptr<Error> Error::lexerError(shared_ptr<Location> location, string lexme) {
    shared_ptr<Error> error = make_shared<Error>();
    error->kind = ErrorKind::LEXER_ERROR;
    error->location = location;
    error->lexme = std::move(lexme);
    return error;
}

shared_ptr<Error> Error::parserError(shared_ptr<Token> actualToken, optional<TokenKind> expectedTokenKind, optional<Parsee> expectedParsee, optional<string> message) {
    shared_ptr<Error> error = make_shared<Error>();
    error->kind = ErrorKind::PARSER_ERROR;
    error->actualToken = actualToken;
    error->expectedTokenKind = expectedTokenKind;
    error->expectedParsee = std::move(expectedParsee);
    error->message = std::move(message);
    return error;
}

shared_ptr<Error> Error::builderFunctionError(string functionName, string message) {
    shared_ptr<Error> error = make_shared<Error>();
    error->kind = ErrorKind::BUILDER_FUNCTION_ERROR;
    error->functionName = std::move(functionName);
    error->message = std::move(message);
    return error;
}

shared_ptr<Error> Error::builderModuleError(string moduleName, string message) {
    shared_ptr<Error> error = make_shared<Error>();
    error->kind = ErrorKind::BUILDER_MODULE_ERROR;
    error->moduleName = std::move(moduleName);
    error->message = std::move(message);
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