#include "Parsee.h"

#include "Lexer/Token.h"

ParseeToken::ParseeToken(TokenKind tokenKind, bool isRequired, bool shouldReturn):
tokenKind(tokenKind), isRequired(isRequired), shouldReturn(shouldReturn) { }

TokenKind ParseeToken::getTokenKind() {
    return tokenKind;
}

bool ParseeToken::getIsRequired() {
    return isRequired;
}

bool ParseeToken::getShouldReturn() {
    return shouldReturn;
}

ParseeTokensGroup::ParseeTokensGroup(bool isRequired, vector<ParseeToken> tokens, optional<ParseeTokensGroup> repeatedGroup):
isRequired(isRequired), tokens(tokens) {
    if (repeatedGroup) {
        this->repeatedGroup = *repeatedGroup;
    } else {
        this->repeatedGroup = {};
    }
 }

bool ParseeTokensGroup::getIsRequired() {
    return isRequired;
}

vector<ParseeToken> ParseeTokensGroup::getTokens() {
    return tokens;
}

optional<reference_wrapper<ParseeTokensGroup>> ParseeTokensGroup::getRepeatedGroup() {
    return repeatedGroup;
}