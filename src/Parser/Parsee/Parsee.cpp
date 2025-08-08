#include "Parsee.h"

#include "ParseeGroup.h"

Parsee Parsee::groupParsee(ParseeGroup group, bool isRequired, bool shouldReturn, bool shouldFailOnNoMatch) {
    Parsee parsee;
    parsee.kind = ParseeKind::GROUP;
    parsee.group = group;
    parsee.isRequired = isRequired;
    parsee.shouldReturn = shouldReturn;
    parsee.shouldFailOnNoMatch = shouldFailOnNoMatch;
    return parsee;
}

Parsee Parsee::repeatedGroupParsee(ParseeGroup repeatedGroup, bool isRequired, bool shouldReturn, bool shouldFailOnNoMatch) {
    Parsee parsee;
    parsee.kind = ParseeKind::REPEATED_GROUP;
    parsee.repeatedGroup = repeatedGroup;
    parsee.isRequired = isRequired;
    parsee.shouldReturn = shouldReturn;
    parsee.shouldFailOnNoMatch = shouldFailOnNoMatch;
    return parsee;
}

Parsee Parsee::tokenParsee(TokenKind tokenKind, bool isRequired, bool shouldReturn, bool shouldFailOnNoMatch) {
    Parsee  parsee;
    parsee.kind = ParseeKind::TOKEN;
    parsee.tokenKind = tokenKind;
    parsee.isRequired = isRequired;
    parsee.shouldReturn = shouldReturn;
    parsee.shouldFailOnNoMatch = shouldFailOnNoMatch;
    return parsee;
}

Parsee Parsee::valueTypeParsee(bool isRequired, bool shouldReturn, bool shouldFailOnNoMatch) {
    Parsee parsee;
    parsee.kind = ParseeKind::VALUE_TYPE;
    parsee.isRequired = isRequired;
    parsee.shouldReturn = shouldReturn;
    parsee.shouldFailOnNoMatch = shouldFailOnNoMatch;
    return parsee;
}

Parsee Parsee::expressionParsee(bool isRequired, bool shouldReturn, bool shouldFailOnNoMatch) {
    Parsee parsee;
    parsee.kind = ParseeKind::EXPRESSION;
    parsee.isRequired = isRequired;
    parsee.shouldReturn = shouldReturn;
    parsee.shouldFailOnNoMatch = shouldFailOnNoMatch;
    return parsee;
}

Parsee::Parsee() { }

optional<ParseeGroup> Parsee::getGroup() {
    return group;
}

optional<ParseeGroup> Parsee::getRepeatedGroup() {
    return repeatedGroup;
}

ParseeKind Parsee::getKind() {
    return kind;
}

TokenKind Parsee::getTokenKind() {
    return tokenKind;
}

bool Parsee::getIsRequired() {
    return isRequired;
}

bool Parsee::getShouldReturn() {
    return shouldReturn;
}

bool Parsee::getShouldFailOnNoMatch() {
    return shouldFailOnNoMatch;
}