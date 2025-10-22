#include "Parsee.h"

Parsee::Parsee() { }

Parsee Parsee::groupParsee(vector<Parsee> groupParsees, Level level, bool shouldReturn) {
    Parsee parsee;
    parsee.kind = ParseeKind::GROUP;
    parsee.groupParsees = groupParsees;
    parsee.level = level;
    parsee.shouldReturn = shouldReturn;
    return parsee;
}

Parsee Parsee::repeatedGroupParsee(vector<Parsee> repeatedParsees, Level level, bool shouldReturn) {
    Parsee parsee;
    parsee.kind = ParseeKind::REPEATED_GROUP;
    parsee.repeatedParsees = repeatedParsees;
    parsee.level = level;
    parsee.shouldReturn = shouldReturn;
    return parsee;
}

Parsee Parsee::orParsee(vector<Parsee> firstParsees, vector<Parsee> secondParsees, Level level, bool shouldReturn) {
    Parsee parsee;
    parsee.kind = ParseeKind::OR;
    parsee.firstParsees = firstParsees;
    parsee.secondParsees = secondParsees;
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

ParseeKind Parsee::getKind() {
    return kind;
}

int Parsee::getTag() {
    return tag;
}

optional<vector<Parsee>> Parsee::getGroupParsees() {
    return groupParsees;
}

optional<vector<Parsee>> Parsee::getRepeatedParsees() {
    return repeatedParsees;
}

optional<vector<Parsee>> Parsee::getFirstParsees() {
    return firstParsees;
}

optional<vector<Parsee>> Parsee::getSecondParsees() {
    return secondParsees;
}

TokenKind Parsee::getTokenKind() {
    return tokenKind;
}

bool Parsee::getShouldIncludeExpressionStatement() {
    return shouldIncludeExpressionStatement;
}

Parsee::Level Parsee::getLevel() {
    return level;
}

bool Parsee::getShouldReturn() {
    return shouldReturn;
}
