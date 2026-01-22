#include "Parsee.h"

Parsee::Parsee() { }

Parsee Parsee::expressionParsee(ParseeLevel level, bool shouldReturn, bool isNumericExpression, int tag) {
    Parsee parsee;
    parsee.kind = ParseeKind::EXPRESSION;
    parsee.tag = tag;
    parsee.level = level;
    parsee.shouldReturn = shouldReturn;
    parsee.isNumericExpression = isNumericExpression;
    return parsee;
}

Parsee Parsee::expressionBlockMultiLineParsee(ParseeLevel level, bool shouldReturn, int tag) {
    Parsee parsee;
    parsee.kind = ParseeKind::EXPRESSION_BLOCK_MULTI_LINE;
    parsee.tag = tag;
    parsee.level = level;
    parsee.shouldReturn = shouldReturn;
    return parsee;
}

Parsee Parsee::expressionBlockSingleLineParsee(ParseeLevel level, bool shouldReturn, int tag) {
    Parsee parsee;
    parsee.kind = ParseeKind::EXPRESSION_BLOCK_SINGLE_LINE;
    parsee.tag = tag;
    parsee.level = level;
    parsee.shouldReturn = shouldReturn;
    return parsee;
}

Parsee Parsee::debug(string debugMessage) {
    Parsee parsee;
    parsee.kind = ParseeKind::DEBUG;
    parsee.debugMessage = debugMessage;
    return parsee;
}

Parsee Parsee::groupParsee(vector<Parsee> groupParsees, ParseeLevel level, bool shouldReturn) {
    Parsee parsee;
    parsee.kind = ParseeKind::GROUP;
    parsee.groupParsees = groupParsees;
    parsee.level = level;
    parsee.shouldReturn = shouldReturn;
    return parsee;
}

Parsee Parsee::ifElseParsee(bool isMultiLine, ParseeLevel level, bool shouldReturn, int tag) {
    Parsee parsee;
    parsee.kind = isMultiLine ? ParseeKind::IF_ELSE_MULTI_LINE : ParseeKind::IF_ELSE_SINGLE_LINE;
    parsee.tag = tag;
    parsee.level = level;
    parsee.shouldReturn = shouldReturn;
    return parsee;
}

Parsee Parsee::oneOfParsee(vector<vector<Parsee>> parsees, ParseeLevel level, bool shouldReturn) {
    Parsee parsee;
    parsee.kind = ParseeKind::ONE_OF;
    parsee.parsees = parsees;
    parsee.level = level;
    parsee.shouldReturn = shouldReturn;
    return parsee;
}

Parsee Parsee::repeatedGroupParsee(vector<Parsee> repeatedParsees, ParseeLevel level, bool shouldReturn) {
    Parsee parsee;
    parsee.kind = ParseeKind::REPEATED_GROUP;
    parsee.repeatedParsees = repeatedParsees;
    parsee.level = level;
    parsee.shouldReturn = shouldReturn;
    return parsee;
}

Parsee Parsee::statementKindsParsee(vector<StatementKind> statementKinds, ParseeLevel level, bool shouldReturn, int tag) {
    Parsee parsee;
    parsee.kind = ParseeKind::STATEMENT_KINDS;
    parsee.statementKinds = statementKinds;
    parsee.level = level;
    parsee.shouldReturn = shouldReturn;
    parsee.tag = tag;
    return parsee;
}

Parsee Parsee::statementBlockMultiLineParsee(ParseeLevel level, bool shouldReturn, int tag) {
    Parsee parsee;
    parsee.kind = ParseeKind::STATEMENT_BLOCK_MULTI_LINE;
    parsee.tag = tag;
    parsee.level = level;
    parsee.shouldReturn = shouldReturn;
    return parsee;
}

Parsee Parsee::statementBlockSingleLineParsee(ParseeLevel level, bool shouldReturn, int tag) {
    Parsee parsee;
    parsee.kind = ParseeKind::STATEMENT_BLOCK_SINGLE_LINE;
    parsee.tag = tag;
    parsee.level = level;
    parsee.shouldReturn = shouldReturn;
    return parsee;
}

Parsee Parsee::tokenParsee(TokenKind tokenKind, ParseeLevel level, bool shouldReturn, int tag) {
    Parsee  parsee;
    parsee.kind = ParseeKind::TOKEN;
    parsee.tag = tag;
    parsee.tokenKind = tokenKind;
    parsee.level = level;
    parsee.shouldReturn = shouldReturn;
    return parsee;
}

Parsee Parsee::valueTypeParsee(ParseeLevel level, bool shouldReturn, int tag) {
    Parsee parsee;
    parsee.kind = ParseeKind::VALUE_TYPE;
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

optional<vector<vector<Parsee>>> Parsee::getParsees() {
    return parsees;
}

optional<vector<StatementKind>> Parsee::getStatementKinds() {
    return statementKinds;
}

TokenKind Parsee::getTokenKind() {
    return tokenKind;
}

bool Parsee::getShouldIncludeExpressionStatement() {
    return shouldIncludeExpressionStatement;
}

bool Parsee::getIsNumericExpression() {
    return isNumericExpression;
}

ParseeLevel Parsee::getLevel() {
    return level;
}

bool Parsee::getShouldReturn() {
    return shouldReturn;
}

string Parsee::getDebugMessage() {
    return debugMessage;
}
