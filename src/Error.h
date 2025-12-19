#ifndef ERROR_H
#define ERROR_H

#include <iostream>

#include "Parser/Parsee/Parsee.h"

class Location;
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
    BUILDER_FUNCTION_ERROR,
    BUILDER_MODULE_ERROR
};

class Error {
private:
    ErrorKind kind;
    shared_ptr<Location> location;
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
    static shared_ptr<Error> error(shared_ptr<Location> location, string message); 

    static shared_ptr<Error> lexerError(shared_ptr<Location> location, string lexme);
    static shared_ptr<Error> parserError(shared_ptr<Token> actualToken, optional<TokenKind> expectedTokenKind, optional<Parsee> expectedParsee, optional<string> message);

    static shared_ptr<Error> builderFunctionError(string funtionName, string message);
    static shared_ptr<Error> builderModuleError(string moduleName, string message);

    Error();

    ErrorKind getKind();
    shared_ptr<Location> getLocation();
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