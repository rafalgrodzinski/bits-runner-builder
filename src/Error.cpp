#include "Error.h"

#include "Parser/ValueType.h"

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

shared_ptr<Error> Error::analyzerTypesInvalidTypeError(int line, int column, shared_ptr<ValueType> actualType, shared_ptr<ValueType> expectedType) {
    shared_ptr<Error> error = make_shared<Error>();
    error->kind = ErrorKind::ANALYZER_TYPE;
    error->line = line;
    error->column = column;
    error->actualType = actualType;
    error->expectedType = expectedType;
    return error;
}

shared_ptr<Error> Error::analyzerTypesInvalidOperationUnary(int line, int column, shared_ptr<ValueType> type, ExpressionUnaryOperation unaryOperation) {
    shared_ptr<Error> error = make_shared<Error>();
    error->kind = ErrorKind::ANALYZER_TYPE_OPERATION_UNARY;
    error->line = line;
    error->column = column;
    error->firstType = type;
    error->unaryOperation = unaryOperation;
    return error;
}

shared_ptr<Error> Error::analyzerTypesInvalidOperationBinary(int line, int column, shared_ptr<ValueType> firstType, shared_ptr<ValueType> secondType, ExpressionBinaryOperation binaryOperation) {
    shared_ptr<Error> error = make_shared<Error>();
    error->kind = ErrorKind::ANALYZER_TYPE_OPERATION_BINARY;
    error->line = line;
    error->column = column;
    error->firstType = firstType;
    error->secondType = secondType;
    error->binaryOperation = binaryOperation;
    return error;
}

shared_ptr<Error> Error::analyzerTypesAlreadyDefined(int line, int column, string identifier) {
    shared_ptr<Error> error = make_shared<Error>();
    error->kind = ErrorKind::ANALYZER_TYPES_ALREADY_DEFINED;
    error->line = line;
    error->column = column;
    error->identifier = identifier;
    return error;
}

shared_ptr<Error> Error::analyzerTypesNotDefined(int line, int column, string identifier) {
    shared_ptr<Error> error = make_shared<Error>();
    error->kind = ErrorKind::ANALYZER_TYPES_NOT_DEFINED;
    error->line = line;
    error->column = column;
    error->identifier = identifier;
    return error;
}

shared_ptr<Error> Error::error(int line, int column, string message) {
    shared_ptr<Error> error = make_shared<Error>();
    error->kind = ErrorKind::MESSAGE;
    error->line = line;
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

optional<int> Error::getLine() {
    return line;
}

optional<int> Error::getColumn() {
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