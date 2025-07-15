#ifndef PARSEE_H
#define PARSEE_H

#include <vector>
#include <optional>

enum class TokenKind;

using namespace std;

class ParseeToken {
private:
    TokenKind tokenKind;
    bool isRequired;
    bool shouldReturn;

public:
    ParseeToken(TokenKind tokenKind, bool isRequired, bool shouldReturn);
    TokenKind getTokenKind();
    bool getIsRequired();
    bool getShouldReturn();
};

class ParseeTokensGroup {
private:
    bool isRequired;
    vector<ParseeToken> tokens;
    optional<reference_wrapper<ParseeTokensGroup>> repeatedGroup;

public:
    ParseeTokensGroup(bool isRequired, vector<ParseeToken> tokens, optional<ParseeTokensGroup> repeatedGroup);
    bool getIsRequired();
    vector<ParseeToken> getTokens();
    optional<reference_wrapper<ParseeTokensGroup>> getRepeatedGroup();
};

#endif