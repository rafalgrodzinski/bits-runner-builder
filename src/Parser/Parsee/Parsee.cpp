#include "Parsee.h"

Parsee Parsee::tokenParsee(TokenKind tokenKind, bool isRequired, bool shouldReturn) {
    Parsee  parsee;
    parsee.kind = ParseeKind::TOKEN;
    parsee.tokenKind = tokenKind;
    parsee.isRequired = isRequired;
    parsee.shouldReturn = shouldReturn;

    return parsee;
}

Parsee Parsee::valueTypeParsee() {
    Parsee parsee;
    parsee.kind = ParseeKind::VALUE_TYPE;
    parsee.shouldReturn = true;

    return parsee;
}

Parsee::Parsee() { }

ParseeKind Parsee::getKind() {
    return kind;
}

TokenKind Parsee::getTokenKind() {
    return tokenKind;
}

bool Parsee::getIsRequired() {
    return isRequired;
}

bool Parsee::getShouldReturn() {
    return shouldReturn;
}