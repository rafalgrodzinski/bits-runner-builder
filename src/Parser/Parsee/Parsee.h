#ifndef PARSEE_H
#define PARSEE_H

#include <memory>
#include <optional>
#include <vector>

enum class TokenKind;

using namespace std;

enum class ParseeKind {
    GROUP,
    REPEATED_GROUP,
    ONE_OF,
    TOKEN,
    VALUE_TYPE,
    STATEMENT,
    STATEMENT_IN_BLOCK,
    EXPRESSION,
    STATEMENT_BLOCK_SINGLE_LINE,
    STATEMENT_BLOCK_MULTI_LINE,
    EXPRESSION_BLOCK_SINGLE_LINE,
    EXPRESSION_BLOCK_MULTI_LINE,
    IF_ELSE
};

enum class ParseeLevel {
    OPTIONAL,
    REQUIRED,
    CRITICAL
};

class Parsee {
private:
    ParseeKind kind;
    int tag;
    optional<vector<Parsee>> groupParsees;
    optional<vector<Parsee>> repeatedParsees;
    optional<vector<vector<Parsee>>> parsees;
    TokenKind tokenKind;
    bool shouldIncludeExpressionStatement;
    bool isNumericExpression;
    ParseeLevel level;
    bool shouldReturn;
    Parsee();

public:
    static Parsee groupParsee(vector<Parsee> groupParsees, ParseeLevel level, bool shouldReturn);
    static Parsee repeatedGroupParsee(vector<Parsee> repeatedParsees, ParseeLevel level, bool shouldReturn);
    static Parsee oneOfParsee(vector<vector<Parsee>> parsees, ParseeLevel level, bool shouldReturn);
    static Parsee tokenParsee(TokenKind tokenKind, ParseeLevel level, bool shouldReturn, int tag = -1);
    static Parsee valueTypeParsee(ParseeLevel level, bool shouldReturn, int tag = -1); 
    static Parsee statementParsee(ParseeLevel level, bool shouldReturn, int tag = -1); 
    static Parsee statementInBlockParsee(bool shouldIncludeExpressionStatement, ParseeLevel level, bool shouldReturn, int tag = -1);
    static Parsee expressionParsee(ParseeLevel level, bool shouldReturn, bool isNumeric, int tag = -1);
    static Parsee statementBlockSingleLineParsee(ParseeLevel level, bool shouldReturn, int tag = -1);
    static Parsee statementBlockMultiLineParsee(ParseeLevel level, bool shouldReturn, int tag = -1);
    static Parsee expressionBlockSingleLineParsee(ParseeLevel level, bool shouldReturn, int tag = -1);
    static Parsee expressionBlockMultiLineParsee(ParseeLevel level, bool shouldReturn, int tag = -1);
    static Parsee ifElseParsee(ParseeLevel level, bool shouldReturn, int tag = -1);

    ParseeKind getKind();
    int getTag();
    optional<vector<Parsee>> getGroupParsees();
    optional<vector<Parsee>> getRepeatedParsees();
    optional<vector<vector<Parsee>>> getParsees();
    TokenKind getTokenKind();
    bool getShouldIncludeExpressionStatement();
    bool getIsNumericExpression();
    ParseeLevel getLevel();
    bool getShouldReturn();
};

#endif