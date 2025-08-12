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
    EXPRESSION
};

class Parsee {
private:
    ParseeKind kind;
    optional<ParseeGroup> group;
    optional<ParseeGroup> repeatedGroup;
    TokenKind tokenKind;
    bool isRequired;
    bool shouldReturn;
    bool shouldFailOnNoMatch;
    Parsee();

public:
    static Parsee groupParsee(ParseeGroup group, bool isRequired, bool shouldReturn, bool shouldFailOnNoMatch);
    static Parsee repeatedGroupParsee(ParseeGroup repeatedGroup, bool isRequired, bool shouldReturn, bool shouldFailOnNoMatch);
    static Parsee tokenParsee(TokenKind tokenKind, bool isRequired, bool shouldReturn, bool shouldFailOnNoMatch);
    static Parsee valueTypeParsee(bool isRequired, bool shouldReturn, bool shouldFailOnNoMatch); 
    static Parsee expressionParsee(bool isRequired, bool shouldReturn, bool shouldFailOnNoMatch);

    ParseeKind getKind();
    optional<ParseeGroup> getGroup();
    optional<ParseeGroup> getRepeatedGroup();
    TokenKind getTokenKind();
    bool getIsRequired();
    bool getShouldReturn();
    bool getShouldFailOnNoMatch();
};

#endif