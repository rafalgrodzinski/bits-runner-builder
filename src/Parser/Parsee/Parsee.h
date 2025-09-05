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
    EXPRESSION,
    OR,
    EXPRESSION_BLOCK_SINGLE_LINE,
    EXPRESSION_BLOCK_MULTI_LINE
};

class Parsee {
private:
    ParseeKind kind;
    int tag;
    optional<ParseeGroup> group;
    optional<ParseeGroup> repeatedGroup;
    optional<ParseeGroup> firstGroup;
    optional<ParseeGroup> secondGroup;
    TokenKind tokenKind;
    bool isRequired;
    bool shouldReturn;
    bool shouldFailOnNoMatch;
    Parsee();

public:
    static Parsee groupParsee(ParseeGroup group, bool isRequired, bool shouldReturn, bool shouldFailOnNoMatch);
    static Parsee repeatedGroupParsee(ParseeGroup repeatedGroup, bool isRequired, bool shouldReturn, bool shouldFailOnNoMatch);
    static Parsee tokenParsee(TokenKind tokenKind, bool isRequired, bool shouldReturn, bool shouldFailOnNoMatch, int tag = -1);
    static Parsee valueTypeParsee(bool isRequired, bool shouldReturn, bool shouldFailOnNoMatch, int tag = -1); 
    static Parsee expressionParsee(bool isRequired, bool shouldReturn, bool shouldFailOnNoMatch, int tag = -1);
    static Parsee orParsee(ParseeGroup firstGroup, ParseeGroup secondGroup, bool isRequired, bool shouldReturn, bool shouldFailOnNoMatch);
    static Parsee expressionBlockSingleLineParsee(bool isRequired, bool shouldReturn, bool shouldFailOnNoMatch, int tag = -1);
    static Parsee expressionBlockMultiLineParsee(bool isRequired, bool shouldReturn, bool shouldFailOnNoMatch, int tag = -1);

    ParseeKind getKind();
    int getTag();
    optional<ParseeGroup> getGroup();
    optional<ParseeGroup> getRepeatedGroup();
    optional<ParseeGroup> getFirstGroup();
    optional<ParseeGroup> getSecondGroup();
    TokenKind getTokenKind();
    bool getIsRequired();
    bool getShouldReturn();
    bool getShouldFailOnNoMatch();
};

#endif