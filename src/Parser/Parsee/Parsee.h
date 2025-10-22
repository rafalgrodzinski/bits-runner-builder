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
    TOKEN,
    VALUE_TYPE,
    STATEMENT,
    STATEMENT_IN_BLOCK,
    EXPRESSION,
    OR,
    STATEMENT_BLOCK_SINGLE_LINE,
    STATEMENT_BLOCK_MULTI_LINE,
    EXPRESSION_BLOCK_SINGLE_LINE,
    EXPRESSION_BLOCK_MULTI_LINE,
    IF_ELSE
};

class Parsee {
public:
    enum class Level {
        OPTIONAL,
        REQUIRED,
        CRITICAL
    };
private:
    ParseeKind kind;
    int tag;
    optional<vector<Parsee>> groupParsees;
    optional<vector<Parsee>> repeatedParsees;
    optional<vector<Parsee>> firstParsees;
    optional<vector<Parsee>> secondParsees;
    TokenKind tokenKind;
    bool shouldIncludeExpressionStatement;
    Level level;
    bool shouldReturn;
    Parsee();

public:
    static Parsee groupParsee(vector<Parsee> groupParsees, Level level, bool shouldReturn);
    static Parsee repeatedGroupParsee(vector<Parsee> repeatedParsees, Level level, bool shouldReturn);
    static Parsee tokenParsee(TokenKind tokenKind, Level level, bool shouldReturn, int tag = -1);
    static Parsee valueTypeParsee(Level level, bool shouldReturn, int tag = -1); 
    static Parsee statementParsee(Level level, bool shouldReturn, int tag = -1); 
    static Parsee statementInBlockParsee(bool shouldIncludeExpressionStatement, Level level, bool shouldReturn, int tag = -1);
    static Parsee expressionParsee(Level level, bool shouldReturn, int tag = -1);
    static Parsee orParsee(vector<Parsee> firstParsees, vector<Parsee> secondParsees, Level level, bool shouldReturn);
    static Parsee statementBlockSingleLineParsee(Level level, bool shouldReturn, int tag = -1);
    static Parsee statementBlockMultiLineParsee(Level level, bool shouldReturn, int tag = -1);
    static Parsee expressionBlockSingleLineParsee(Level level, bool shouldReturn, int tag = -1);
    static Parsee expressionBlockMultiLineParsee(Level level, bool shouldReturn, int tag = -1);
    static Parsee ifElseParsee(Level level, bool shouldReturn, int tag = -1);

    ParseeKind getKind();
    int getTag();
    optional<vector<Parsee>> getGroupParsees();
    optional<vector<Parsee>> getRepeatedParsees();
    optional<vector<Parsee>> getFirstParsees();
    optional<vector<Parsee>> getSecondParsees();
    TokenKind getTokenKind();
    bool getShouldIncludeExpressionStatement();
    bool getIsRequired();
    bool getShouldReturn();
    bool getShouldFailOnNoMatch();
    Level getLevel();
};

#endif