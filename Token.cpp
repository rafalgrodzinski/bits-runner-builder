#include "Token.h"

Token::Token(Kind kind): kind(kind) {
}

std::string Token::toString() {
    switch (kind) {
        case integer:
            return "INTEGER";
            break;
        case real:
            return "REAL";
            break;
        case eof:
            return "EOF";
            break;
    }
}