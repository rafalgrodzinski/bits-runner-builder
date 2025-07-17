#ifndef PARSEE_H
#define PARSEE_H

enum class TokenKind;

enum class ParseeKind {
    TOKEN,
    TYPE
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
    static Parsee typeParsee();

    ParseeKind getKind();

    TokenKind getTokenKind();
    bool getIsRequired();
    bool getShouldReturn();
};

#endif