#ifndef ERROR_H
#define ERROR_H

#include <iostream>

#include "Parser/Parsee/Parsee.h"

class Token;
class ValueType;
enum class TokenKind;

using namespace std;

enum class ErrorKind {
    LEXER_ERROR,
    PARSER_ERROR,
    ANALYZER_TYPE,
    BUILDER_ERROR,
    BUILDER_FUNCTION_ERROR,
    BUILDER_MODULE_ERROR
};

class Error {
private:
    ErrorKind kind;
    optional<int> line;
    optional<int> column;
    optional<string>  lexme;

    shared_ptr<Token> actualToken;
    optional<TokenKind> expectedTokenKind;
    optional<Parsee> expectedParsee;

    shared_ptr<ValueType> actualType;
    shared_ptr<ValueType> expectedType;

    optional<string> functionName;
    optional<string> moduleName;
    optional<string> message;

public:
    static shared_ptr<Error> lexerError(int line, int column, string lexme);
    static shared_ptr<Error> parserError(shared_ptr<Token> actualToken, optional<TokenKind> expectedTokenKind, optional<Parsee> expectedParsee, optional<string> message);
    static shared_ptr<Error> builderError(int line, int column, string message);
    static shared_ptr<Error> analyzerTypeError(int line, int column, shared_ptr<ValueType> actualType, shared_ptr<ValueType> expectedType);
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

    optional<string> getFunctionName();
    optional<string> getModuleName();
    optional<string> getMessage();
};

#endif