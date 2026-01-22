#ifndef PARSEE_H
#define PARSEE_H

#include <memory>
#include <optional>
#include <vector>

enum class TokenKind;
enum class StatementKind;

using namespace std;

enum class ParseeKind {
    EXPRESSION,
    EXPRESSION_BLOCK_MULTI_LINE,
    EXPRESSION_BLOCK_SINGLE_LINE,
    DEBUG,
    GROUP,
    IF_ELSE_MULTI_LINE,
    IF_ELSE_SINGLE_LINE,
    ONE_OF,
    REPEATED_GROUP,
    STATEMENT_KINDS,
    STATEMENT_BLOCK_MULTI_LINE,
    STATEMENT_BLOCK_SINGLE_LINE,
    STATEMENT_IN_BLOCK,
    TOKEN,
    VALUE_TYPE
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
    optional<vector<StatementKind>> statementKinds;
    TokenKind tokenKind;
    bool shouldIncludeExpressionStatement;
    bool isNumericExpression;
    ParseeLevel level;
    bool shouldReturn;
    string debugMessage;
    Parsee();

public:
    static Parsee expressionParsee(ParseeLevel level, bool shouldReturn, bool isNumeric, int tag = -1);
    static Parsee expressionBlockMultiLineParsee(ParseeLevel level, bool shouldReturn, int tag = -1);
    static Parsee expressionBlockSingleLineParsee(ParseeLevel level, bool shouldReturn, int tag = -1);
    static Parsee debug(string debugMessage);
    static Parsee groupParsee(vector<Parsee> groupParsees, ParseeLevel level, bool shouldReturn);
    static Parsee ifElseParsee(bool isMultiLine, ParseeLevel level, bool shouldReturn, int tag = -1);
    static Parsee oneOfParsee(vector<vector<Parsee>> parsees, ParseeLevel level, bool shouldReturn);
    static Parsee repeatedGroupParsee(vector<Parsee> repeatedParsees, ParseeLevel level, bool shouldReturn);
    static Parsee statementKindsParsee(vector<StatementKind> statementKinds, ParseeLevel level, bool shouldReturn, int tag = -1);
    static Parsee statementBlockMultiLineParsee(ParseeLevel level, bool shouldReturn, int tag = -1);
    static Parsee statementBlockSingleLineParsee(ParseeLevel level, bool shouldReturn, int tag = -1);
    static Parsee statementInBlockParsee(bool shouldIncludeExpressionStatement, ParseeLevel level, bool shouldReturn, int tag = -1);
    static Parsee tokenParsee(TokenKind tokenKind, ParseeLevel level, bool shouldReturn, int tag = -1);
    static Parsee valueTypeParsee(ParseeLevel level, bool shouldReturn, int tag = -1); 

    ParseeKind getKind();
    int getTag();
    optional<vector<Parsee>> getGroupParsees();
    optional<vector<Parsee>> getRepeatedParsees();
    optional<vector<vector<Parsee>>> getParsees();
    optional<vector<StatementKind>> getStatementKinds();
    TokenKind getTokenKind();
    bool getShouldIncludeExpressionStatement();
    bool getIsNumericExpression();
    ParseeLevel getLevel();
    bool getShouldReturn();
    string getDebugMessage();
};

#endif