#include "Parsee.h"

#include "ParseeGroup.h"

Parsee Parsee::groupParsee(ParseeGroup group, Level level, bool shouldReturn) {
    Parsee parsee;
    parsee.kind = ParseeKind::GROUP;
    parsee.group = group;
    parsee.level = level;
    parsee.shouldReturn = shouldReturn;
    return parsee;
}

Parsee Parsee::repeatedGroupParsee(ParseeGroup repeatedGroup, Level level, bool shouldReturn) {
    Parsee parsee;
    parsee.kind = ParseeKind::REPEATED_GROUP;
    parsee.repeatedGroup = repeatedGroup;
    parsee.level = level;
    parsee.shouldReturn = shouldReturn;
    return parsee;
}

Parsee Parsee::tokenParsee(TokenKind tokenKind, Level level, bool shouldReturn, int tag) {
    Parsee  parsee;
    parsee.kind = ParseeKind::TOKEN;
    parsee.tag = tag;
    parsee.tokenKind = tokenKind;
    parsee.level = level;
    parsee.shouldReturn = shouldReturn;
    return parsee;
}

Parsee Parsee::valueTypeParsee(Level level, bool shouldReturn, int tag) {
    Parsee parsee;
    parsee.kind = ParseeKind::VALUE_TYPE;
    parsee.tag = tag;
    parsee.level = level;
    parsee.shouldReturn = shouldReturn;
    return parsee;
}

Parsee Parsee::statementParsee(Level level, bool shouldReturn, int tag) {
    Parsee parsee;
    parsee.kind = ParseeKind::STATEMENT;
    parsee.tag = tag;
    parsee.level = level;
    parsee.shouldReturn = shouldReturn;
    return parsee;
}

Parsee Parsee::statementInBlockParsee(bool shouldIncludeExpressionStatement, Level level, bool shouldReturn, int tag) {
    Parsee parsee;
    parsee.kind = ParseeKind::STATEMENT_IN_BLOCK;
    parsee.tag = tag;
    parsee.shouldIncludeExpressionStatement = shouldIncludeExpressionStatement;
    parsee.level = level;
    parsee.shouldReturn = shouldReturn;
    return parsee;
}

Parsee Parsee::expressionParsee(Level level, bool shouldReturn, int tag) {
    Parsee parsee;
    parsee.kind = ParseeKind::EXPRESSION;
    parsee.tag = tag;
    parsee.level = level;
    parsee.shouldReturn = shouldReturn;
    return parsee;
}

Parsee Parsee::orParsee(ParseeGroup firstGroup, ParseeGroup secondGroup, Level level, bool shouldReturn) {
    Parsee parsee;
    parsee.kind = ParseeKind::OR;
    parsee.firstGroup = firstGroup;
    parsee.secondGroup = secondGroup;
    parsee.level = level;
    parsee.shouldReturn = shouldReturn;
    return parsee;
}

Parsee Parsee::statementBlockSingleLineParsee(Level level, bool shouldReturn, int tag) {
    Parsee parsee;
    parsee.kind = ParseeKind::STATEMENT_BLOCK_SINGLE_LINE;
    parsee.tag = tag;
    parsee.level = level;
    parsee.shouldReturn = shouldReturn;
    return parsee;
}

Parsee Parsee::statementBlockMultiLineParsee(Level level, bool shouldReturn, int tag) {
    Parsee parsee;
    parsee.kind = ParseeKind::STATEMENT_BLOCK_MULTI_LINE;
    parsee.tag = tag;
    parsee.level = level;
    parsee.shouldReturn = shouldReturn;
    return parsee;
}

Parsee Parsee::expressionBlockSingleLineParsee(Level level, bool shouldReturn, int tag) {
    Parsee parsee;
    parsee.kind = ParseeKind::EXPRESSION_BLOCK_SINGLE_LINE;
    parsee.tag = tag;
    parsee.level = level;
    parsee.shouldReturn = shouldReturn;
    return parsee;
}

Parsee Parsee::expressionBlockMultiLineParsee(Level level, bool shouldReturn, int tag) {
    Parsee parsee;
    parsee.kind = ParseeKind::EXPRESSION_BLOCK_MULTI_LINE;
    parsee.tag = tag;
    parsee.level = level;
    parsee.shouldReturn = shouldReturn;
    return parsee;
}
Parsee Parsee::ifElseParsee(Level level, bool shouldReturn, int tag) {
    Parsee parsee;
    parsee.kind = ParseeKind::IF_ELSE;
    parsee.tag = tag;
    parsee.level = level;
    parsee.shouldReturn = shouldReturn;
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
    return level == Level::REQUIRED;
}

bool Parsee::getShouldReturn() {
    return shouldReturn;
}

bool Parsee::getShouldFailOnNoMatch() {
    return level == Level::CRITICAL;
}