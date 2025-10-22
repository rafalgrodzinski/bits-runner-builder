#ifndef PARSEE_H
#define PARSEE_H

#include <memory>
#include <optional>

#include "ParseeGroup.h"

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
    optional<ParseeGroup> group;
    optional<ParseeGroup> repeatedGroup;
    optional<ParseeGroup> firstGroup;
    optional<ParseeGroup> secondGroup;
    TokenKind tokenKind;
    bool shouldIncludeExpressionStatement;
    Level level;
    bool shouldReturn;
    Parsee();

public:
    static Parsee groupParsee(ParseeGroup group, Level level, bool shouldReturn);
    static Parsee repeatedGroupParsee(ParseeGroup repeatedGroup, Level level, bool shouldReturn);
    static Parsee tokenParsee(TokenKind tokenKind, Level level, bool shouldReturn, int tag = -1);
    static Parsee valueTypeParsee(Level level, bool shouldReturn, int tag = -1); 
    static Parsee statementParsee(Level level, bool shouldReturn, int tag = -1); 
    static Parsee statementInBlockParsee(bool shouldIncludeExpressionStatement, Level level, bool shouldReturn, int tag = -1);
    static Parsee expressionParsee(Level level, bool shouldReturn, int tag = -1);
    static Parsee orParsee(ParseeGroup firstGroup, ParseeGroup secondGroup, Level level, bool shouldReturn);
    static Parsee statementBlockSingleLineParsee(Level level, bool shouldReturn, int tag = -1);
    static Parsee statementBlockMultiLineParsee(Level level, bool shouldReturn, int tag = -1);
    static Parsee expressionBlockSingleLineParsee(Level level, bool shouldReturn, int tag = -1);
    static Parsee expressionBlockMultiLineParsee(Level level, bool shouldReturn, int tag = -1);
    static Parsee ifElseParsee(Level level, bool shouldReturn, int tag = -1);

    ParseeKind getKind();
    int getTag();
    optional<ParseeGroup> getGroup();
    optional<ParseeGroup> getRepeatedGroup();
    optional<ParseeGroup> getFirstGroup();
    optional<ParseeGroup> getSecondGroup();
    TokenKind getTokenKind();
    bool getShouldIncludeExpressionStatement();
    bool getIsRequired();
    bool getShouldReturn();
    bool getShouldFailOnNoMatch();
    Level getLevel();
};

#endif