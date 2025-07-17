#ifndef PARSEE_H
#define PARSEE_H

enum class TokenKind;

enum class ParseeKind {
    TOKEN,
    VALUE_TYPE
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
    static Parsee valueTypeParsee();

    ParseeKind getKind();
    TokenKind getTokenKind();
    bool getIsRequired();
    bool getShouldReturn();
};

#endif