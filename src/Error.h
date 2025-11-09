#ifndef ERROR_H
#define ERROR_H

#include <iostream>

#include "Parser/Parsee/Parsee.h"

class Token;
class ValueType;

enum class ExpressionUnaryOperation;
enum class ExpressionBinaryOperation;
enum class TokenKind;

using namespace std;

enum class ErrorKind {
    MESSAGE,
    LEXER_ERROR,
    PARSER_ERROR,
    ANALYZER_TYPE,
    ANALYZER_TYPE_OPERATION_UNARY,
    ANALYZER_TYPE_OPERATION_BINARY,
    ANALYZER_TYPES_ALREADY_DEFINED,
    ANALYZER_TYPES_NOT_DEFINED,
    BUILDER_ERROR,
    BUILDER_FUNCTION_ERROR,
    BUILDER_MODULE_ERROR
};

class Error {
private:
    ErrorKind kind;
    optional<int> line;
    optional<int> column;
    optional<string> lexme;

    shared_ptr<Token> actualToken;
    optional<TokenKind> expectedTokenKind;
    optional<Parsee> expectedParsee;

    shared_ptr<ValueType> actualType;
    shared_ptr<ValueType> expectedType;

    shared_ptr<ValueType> firstType;
    shared_ptr<ValueType> secondType;
    optional<ExpressionUnaryOperation> unaryOperation;
    optional<ExpressionBinaryOperation> binaryOperation;
    string identifier;

    optional<string> functionName;
    optional<string> moduleName;
    optional<string> message;

public:
    static shared_ptr<Error> lexerError(int line, int column, string lexme);
    static shared_ptr<Error> parserError(shared_ptr<Token> actualToken, optional<TokenKind> expectedTokenKind, optional<Parsee> expectedParsee, optional<string> message);
    static shared_ptr<Error> builderError(int line, int column, string message);

    static shared_ptr<Error> analyzerTypesInvalidTypeError(int line, int column, shared_ptr<ValueType> actualType, shared_ptr<ValueType> expectedType);
    static shared_ptr<Error> analyzerTypesInvalidOperationUnary(int line, int column, shared_ptr<ValueType> type, ExpressionUnaryOperation unaryOperation);
    static shared_ptr<Error> analyzerTypesInvalidOperationBinary(int line, int column, shared_ptr<ValueType> firstType, shared_ptr<ValueType> secondType, ExpressionBinaryOperation binaryOperation);
    static shared_ptr<Error> analyzerTypesAlreadyDefined(int line, int column, string identifier);
    static shared_ptr<Error> analyzerTypesNotDefined(int line, int column, string identifier);
    static shared_ptr<Error> error(int line, int column, string message); 

    static shared_ptr<Error> builderFunctionError(string funtionName, string message);
    static shared_ptr<Error> builderModuleError(string moduleName, string message);

    Error();

    ErrorKind getKind();
    optional<int> getLine();
    optional<int> getColumn();
    optional<string> getLexme();

    shared_ptr<Token> getActualToken();
    optional<TokenKind> getExpectedTokenKind();
    optional<Parsee> getExpectedParsee();

    shared_ptr<ValueType> getActualType();
    shared_ptr<ValueType> getExpectedType();

    shared_ptr<ValueType> getFirstType();
    shared_ptr<ValueType> getSecondType();
    optional<ExpressionUnaryOperation> getUnaryOperation();
    optional<ExpressionBinaryOperation> getBinaryOperation();
    optional<string> getIdentifier();

    optional<string> getFunctionName();
    optional<string> getModuleName();
    optional<string> getMessage();
};

#endif