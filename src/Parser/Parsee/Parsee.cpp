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

Parsee Parsee::tokenParsee(TokenKind tokenKind, bool isRequired, bool shouldReturn, bool shouldFailOnNoMatch, int tag) {
    Parsee  parsee;
    parsee.kind = ParseeKind::TOKEN;
    parsee.tag = tag;
    parsee.tokenKind = tokenKind;
    parsee.isRequired = isRequired;
    parsee.shouldReturn = shouldReturn;
    parsee.shouldFailOnNoMatch = shouldFailOnNoMatch;
    return parsee;
}

Parsee Parsee::valueTypeParsee(bool isRequired, bool shouldReturn, bool shouldFailOnNoMatch, int tag) {
    Parsee parsee;
    parsee.kind = ParseeKind::VALUE_TYPE;
    parsee.tag = tag;
    parsee.isRequired = isRequired;
    parsee.shouldReturn = shouldReturn;
    parsee.shouldFailOnNoMatch = shouldFailOnNoMatch;
    return parsee;
}

Parsee Parsee::statementParsee(bool isRequired, bool shouldReturn, bool shouldFailOnNoMatch, int tag) {
    Parsee parsee;
    parsee.kind = ParseeKind::STATEMENT;
    parsee.tag = tag;
    parsee.isRequired = isRequired;
    parsee.shouldReturn = shouldReturn;
    parsee.shouldFailOnNoMatch = shouldFailOnNoMatch;
    return parsee;
}

Parsee Parsee::statementInBlockParsee(bool shouldIncludeExpressionStatement, bool isRequired, bool shouldReturn, bool shouldFailOnNoMatch, int tag) {
    Parsee parsee;
    parsee.kind = ParseeKind::STATEMENT_IN_BLOCK;
    parsee.tag = tag;
    parsee.shouldIncludeExpressionStatement = shouldIncludeExpressionStatement;
    parsee.isRequired = isRequired;
    parsee.shouldReturn = shouldReturn;
    parsee.shouldFailOnNoMatch = shouldFailOnNoMatch;
    return parsee;
}

Parsee Parsee::expressionParsee(bool isRequired, bool shouldReturn, bool shouldFailOnNoMatch, int tag) {
    Parsee parsee;
    parsee.kind = ParseeKind::EXPRESSION;
    parsee.tag = tag;
    parsee.isRequired = isRequired;
    parsee.shouldReturn = shouldReturn;
    parsee.shouldFailOnNoMatch = shouldFailOnNoMatch;
    return parsee;
}

Parsee Parsee::orParsee(ParseeGroup firstGroup, ParseeGroup secondGroup, bool isRequired, bool shouldReturn, bool shouldFailOnNoMatch) {
    Parsee parsee;
    parsee.kind = ParseeKind::OR;
    parsee.firstGroup = firstGroup;
    parsee.secondGroup = secondGroup;
    parsee.isRequired = isRequired;
    parsee.shouldReturn = shouldReturn;
    parsee.shouldFailOnNoMatch = shouldFailOnNoMatch;
    return parsee;
}

Parsee Parsee::statementBlockSingleLineParsee(bool isRequired, bool shouldReturn, bool shouldFailOnNoMatch, int tag) {
    Parsee parsee;
    parsee.kind = ParseeKind::STATEMENT_BLOCK_SINGLE_LINE;
    parsee.tag = tag;
    parsee.isRequired = isRequired;
    parsee.shouldReturn = shouldReturn;
    parsee.shouldFailOnNoMatch = shouldFailOnNoMatch;
    return parsee;
}

Parsee Parsee::statementBlockMultiLineParsee(bool isRequired, bool shouldReturn, bool shouldFailOnNoMatch, int tag) {
    Parsee parsee;
    parsee.kind = ParseeKind::STATEMENT_BLOCK_MULTI_LINE;
    parsee.tag = tag;
    parsee.isRequired = isRequired;
    parsee.shouldReturn = shouldReturn;
    parsee.shouldFailOnNoMatch = shouldFailOnNoMatch;
    return parsee;
}

Parsee Parsee::expressionBlockSingleLineParsee(bool isRequired, bool shouldReturn, bool shouldFailOnNoMatch, int tag) {
    Parsee parsee;
    parsee.kind = ParseeKind::EXPRESSION_BLOCK_SINGLE_LINE;
    parsee.tag = tag;
    parsee.isRequired = isRequired;
    parsee.shouldReturn = shouldReturn;
    parsee.shouldFailOnNoMatch = shouldFailOnNoMatch;
    return parsee;
}

Parsee Parsee::expressionBlockMultiLineParsee(bool isRequired, bool shouldReturn, bool shouldFailOnNoMatch, int tag) {
    Parsee parsee;
    parsee.kind = ParseeKind::EXPRESSION_BLOCK_MULTI_LINE;
    parsee.tag = tag;
    parsee.isRequired = isRequired;
    parsee.shouldReturn = shouldReturn;
    parsee.shouldFailOnNoMatch = shouldFailOnNoMatch;
    return parsee;
}
Parsee Parsee::ifElseParsee(bool isRequired, bool shouldReturn, bool shouldFailOnNoMatch, int tag) {
    Parsee parsee;
    parsee.kind = ParseeKind::IF_ELSE;
    parsee.tag = tag;
    parsee.isRequired = isRequired;
    parsee.shouldReturn = shouldReturn;
    parsee.shouldFailOnNoMatch = shouldFailOnNoMatch;
    return parsee;
}

Parsee::Parsee() { }

ParseeKind Parsee::getKind() {
    return kind;
}

int Parsee::getTag() {
    return tag;
}

optional<ParseeGroup> Parsee::getGroup() {
    return group;
}

optional<ParseeGroup> Parsee::getRepeatedGroup() {
    return repeatedGroup;
}

optional<ParseeGroup> Parsee::getFirstGroup() {
    return firstGroup;
}

optional<ParseeGroup> Parsee::getSecondGroup() {
    return secondGroup;
}

TokenKind Parsee::getTokenKind() {
    return tokenKind;
}

bool Parsee::getShouldIncludeExpressionStatement() {
    return shouldIncludeExpressionStatement;
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