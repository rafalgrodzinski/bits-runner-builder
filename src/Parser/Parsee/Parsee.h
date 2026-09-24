#ifndef PARSEE_H
#define PARSEE_H

#include <memory>
#include <optional>
#include <string>
#include <vector>

enum class TokenKind;
enum class StatementKind;

enum class ParseeKind {
    EXPRESSION,
    EXPRESSION_BLOCK_MULTI_LINE,
    EXPRESSION_BLOCK_SINGLE_LINE,
    DEBUG,
    GROUP,
    IF_ELSE,
    IF_ELSE_MULTI_LINE,
    IF_ELSE_SINGLE_LINE,
    ONE_OF,
    REPEATED_GROUP,
    STATEMENT_KINDS,
    STATEMENT_BLOCK_MULTI_LINE,
    STATEMENT_BLOCK_SINGLE_LINE,
    TOKEN,
    VALUE_TYPE
};

enum class ParseeLevel {
    OPTIONAL,
    REQUIRED,
    CRITICAL
};

class Parsee {
public:
    static Parsee debug(const std::string &debugMessage);
    static Parsee expressionParsee(ParseeLevel level, bool shouldReturn, bool isNumeric, int tag = -1);
    static Parsee expressionBlockMultiLineParsee(ParseeLevel level, bool shouldReturn, int tag = -1);
    static Parsee expressionBlockSingleLineParsee(ParseeLevel level, bool shouldReturn, int tag = -1);
    static Parsee groupParsee(const std::vector<Parsee> &groupParsees, ParseeLevel level, bool shouldReturn);
    static Parsee ifElseParsee(std::optional<bool> isMultiLine, ParseeLevel level, bool shouldReturn, int tag = -1);
    static Parsee oneOfParsee(const std::vector<std::vector<Parsee>> &parsees, ParseeLevel level, bool shouldReturn);
    static Parsee repeatedGroupParsee(const std::vector<Parsee> &repeatedParsees, ParseeLevel level, bool shouldReturn);
    static Parsee statementKindsParsee(const std::vector<StatementKind> &statementKinds, ParseeLevel level, bool shouldReturn, int tag = -1);
    static Parsee statementBlockMultiLineParsee(ParseeLevel level, bool shouldReturn, int tag = -1);
    static Parsee statementBlockSingleLineParsee(ParseeLevel level, bool shouldReturn, int tag = -1);
    static Parsee tokenParsee(TokenKind tokenKind, ParseeLevel level, bool shouldReturn, int tag = -1);
    static Parsee valueTypeParsee(ParseeLevel level, bool shouldReturn, int tag = -1); 

    ParseeKind getKind() const;
    int getTag() const;
    std::optional<std::vector<Parsee>> getGroupParsees() const;
    std::optional<std::vector<Parsee>> getRepeatedParsees() const;
    std::optional<std::vector<std::vector<Parsee>>> getParsees() const;
    std::optional<std::vector<StatementKind>> getStatementKinds() const;
    TokenKind getTokenKind() const;
    bool getShouldIncludeExpressionStatement() const;
    bool getIsNumericExpression() const;
    ParseeLevel getLevel() const;
    bool getShouldReturn() const;
    std::string getDebugMessage() const;

private:
    ParseeKind kind;
    int tag;
    std::optional<std::vector<Parsee>> groupParsees;
    std::optional<std::vector<Parsee>> repeatedParsees;
    std::optional<std::vector<std::vector<Parsee>>> parsees;
    std::optional<std::vector<StatementKind>> statementKinds;
    TokenKind tokenKind;
    bool shouldIncludeExpressionStatement;
    bool isNumericExpression;
    ParseeLevel level;
    bool shouldReturn;
    std::string debugMessage;
    Parsee();
};

#endif