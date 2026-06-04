#include "Parsee.h"

Parsee::Parsee() { }

Parsee Parsee::debug(const string &debugMessage) {
    Parsee parsee;
    parsee.kind = ParseeKind::DEBUG;
    parsee.debugMessage = std::move(debugMessage);
    return parsee;
}

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

Parsee Parsee::groupParsee(vector<Parsee> groupParsees, ParseeLevel level, bool shouldReturn) {
    Parsee parsee;
    parsee.kind = ParseeKind::GROUP;
    parsee.groupParsees = groupParsees;
    parsee.level = level;
    parsee.shouldReturn = shouldReturn;
    return parsee;
}

Parsee Parsee::ifElseParsee(optional<bool> isMultiLine, ParseeLevel level, bool shouldReturn, int tag) {
    Parsee parsee;
    if (isMultiLine) {
        parsee.kind = *isMultiLine ? ParseeKind::IF_ELSE_MULTI_LINE : ParseeKind::IF_ELSE_SINGLE_LINE;
    } else {
        parsee.kind = ParseeKind::IF_ELSE;
    }
    parsee.tag = tag;
    parsee.level = level;
    parsee.shouldReturn = shouldReturn;
    return parsee;
}

Parsee Parsee::oneOfParsee(const vector<vector<Parsee>> &parsees, ParseeLevel level, bool shouldReturn) {
    Parsee parsee;
    parsee.kind = ParseeKind::ONE_OF;
    parsee.parsees = std::move(parsees);
    parsee.level = level;
    parsee.shouldReturn = shouldReturn;
    return parsee;
}

Parsee Parsee::repeatedGroupParsee(const vector<Parsee> &repeatedParsees, ParseeLevel level, bool shouldReturn) {
    Parsee parsee;
    parsee.kind = ParseeKind::REPEATED_GROUP;
    parsee.repeatedParsees = std::move(repeatedParsees);
    parsee.level = level;
    parsee.shouldReturn = shouldReturn;
    return parsee;
}

Parsee Parsee::statementKindsParsee(const vector<StatementKind> &statementKinds, ParseeLevel level, bool shouldReturn, int tag) {
    Parsee parsee;
    parsee.kind = ParseeKind::STATEMENT_KINDS;
    parsee.statementKinds = std::move(statementKinds);
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

ParseeKind Parsee::getKind() const {
    return kind;
}

int Parsee::getTag() const {
    return tag;
}

optional<vector<Parsee>> Parsee::getGroupParsees() const {
    return groupParsees;
}

optional<vector<Parsee>> Parsee::getRepeatedParsees() const {
    return repeatedParsees;
}

optional<vector<vector<Parsee>>> Parsee::getParsees() const {
    return parsees;
}

optional<vector<StatementKind>> Parsee::getStatementKinds() const {
    return statementKinds;
}

TokenKind Parsee::getTokenKind() const {
    return tokenKind;
}

bool Parsee::getShouldIncludeExpressionStatement() const {
    return shouldIncludeExpressionStatement;
}

bool Parsee::getIsNumericExpression() const {
    return isNumericExpression;
}

ParseeLevel Parsee::getLevel() const {
    return level;
}

bool Parsee::getShouldReturn() const {
    return shouldReturn;
}

string Parsee::getDebugMessage() const {
    return debugMessage;
}
