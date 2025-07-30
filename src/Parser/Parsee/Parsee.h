#ifndef PARSEE_H
#define PARSEE_H

#include <memory>

enum class TokenKind;

enum class ParseeKind {
    TOKEN,
    VALUE_TYPE,
    EXPRESSION
};

class Parsee {
private:
    ParseeKind kind;
    TokenKind tokenKind;
    bool isRequired;
    bool shouldReturn;
    Parsee();

public:
    static Parsee tokenParsee(TokenKind tokenKind, bool isRequired, bool shouldReturn);
    static Parsee valueTypeParsee(bool isRequired);
    static Parsee expressionParsee(bool isRequired);

    ParseeKind getKind();
    TokenKind getTokenKind();
    bool getIsRequired();
    bool getShouldReturn();
};

#endif