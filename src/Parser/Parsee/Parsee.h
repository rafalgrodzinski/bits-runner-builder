#ifndef PARSEE_H
#define PARSEE_H

#include <memory>
#include <optional>

#include "ParseeGroup.h"

enum class TokenKind;
//class ParseeGroup;

using namespace std;

enum class ParseeKind {
    REPEATED_GROUP,
    TOKEN,
    VALUE_TYPE,
    EXPRESSION
};

class Parsee {
private:
    ParseeKind kind;
    optional<ParseeGroup> repeatedGroup;
    TokenKind tokenKind;
    bool isRequired;
    bool shouldReturn;
    bool shouldFailOnNoMatch;
    Parsee();

public:
    static Parsee repeatedGroupParsee(ParseeGroup repeatedGroup, bool isRequired, bool shouldReturn, bool shouldFailOnNoMatch);
    static Parsee tokenParsee(TokenKind tokenKind, bool isRequired, bool shouldReturn);
    static Parsee valueTypeParsee(bool isRequired); 
    static Parsee expressionParsee(bool isRequired);

    ParseeKind getKind();
    optional<ParseeGroup> getRepeatedGroup();
    TokenKind getTokenKind();
    bool getIsRequired();
    bool getShouldReturn();
    bool getShouldFailOnNoMatch();
};

#endif