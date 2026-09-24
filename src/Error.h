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

enum class ErrorKind {
    MESSAGE,
    LEXER_ERROR,
    PARSER_ERROR,
    BUILDER_FUNCTION_ERROR,
    BUILDER_MODULE_ERROR
};

class Error {
public:
    static std::shared_ptr<Error> error(std::shared_ptr<Location> location, const std::string &message); 

    static std::shared_ptr<Error> lexerError(std::shared_ptr<Location> location, const std::string &lexme);
    static std::shared_ptr<Error> parserError(std::shared_ptr<Token> actualToken, std::optional<TokenKind> expectedTokenKind, const std::optional<Parsee> &expectedParsee, const std::optional<std::string> &message);

    static std::shared_ptr<Error> builderFunctionError(const std::string &funtionName, const std::string &message);
    static std::shared_ptr<Error> builderModuleError(const std::string &moduleName, const std::string &message);

    Error();

    ErrorKind getKind() const;
    std::shared_ptr<Location> getLocation() const;
    std::optional<std::string> getLexme() const;

    std::shared_ptr<Token> getActualToken() const;
    std::optional<TokenKind> getExpectedTokenKind() const;
    std::optional<Parsee> getExpectedParsee() const;

    std::shared_ptr<ValueType> getActualType() const;
    std::shared_ptr<ValueType> getExpectedType() const;

    std::shared_ptr<ValueType> getFirstType() const;
    std::shared_ptr<ValueType> getSecondType() const;
    std::optional<ExpressionUnaryOperation> getUnaryOperation() const;
    std::optional<ExpressionBinaryOperation> getBinaryOperation() const ;
    std::optional<std::string> getIdentifier()const;

    std::optional<std::string> getFunctionName() const;
    std::optional<std::string> getModuleName() const;
    std::optional<std::string> getMessage() const;

private:
    ErrorKind kind;
    std::shared_ptr<Location> location;
    std::optional<std::string> lexme;

    std::shared_ptr<Token> actualToken;
    std::optional<TokenKind> expectedTokenKind;
    std::optional<Parsee> expectedParsee;

    std::shared_ptr<ValueType> actualType;
    std::shared_ptr<ValueType> expectedType;

    std::shared_ptr<ValueType> firstType;
    std::shared_ptr<ValueType> secondType;
    std::optional<ExpressionUnaryOperation> unaryOperation;
    std::optional<ExpressionBinaryOperation> binaryOperation;
    std::string identifier;

    std::optional<std::string> functionName;
    std::optional<std::string> moduleName;
    std::optional<std::string> message;
};

#endif