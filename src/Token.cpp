#include "Token.h"

Token::Token(Kind kind, string lexme, int line, int column):
    kind(kind), lexme(lexme), line(line), column(column) {
}

Token::Kind Token::getKind() {
    return kind;
}

string Token::getLexme() {
    return lexme;
}

int Token::getLine() {
    return line;
}

int Token::getColumn() {
    return column;
}

bool Token::isValid() {
    return kind != Token::Kind::INVALID;
}

bool Token::isOfKind(vector<Kind> kinds) {
    for (Kind &kind : kinds) {
        if (kind == this->kind)
            return true;
    }
        
    return false;
}

string Token::toString() {
    switch (kind) {
        case PLUS:
            return "PLUS";
        case MINUS:
            return "MINUS";
        case STAR:
            return "STAR";
        case SLASH:
            return "SLASH";
        case PERCENT:
            return "PERCENT";
        
        case EQUAL:
            return "EQUAL";
        case NOT_EQUAL:
            return "NOT_EQUAL";
        case LESS:
            return "LESS";
        case LESS_EQUAL:
            return "LESS_EQUAL";
        case GREATER:
            return "GREATER";
        case GREATER_EQUAL:
            return "GREATER_EQUAL";

        case LEFT_PAREN:
            return "LEFT_PAREN";
        case RIGHT_PAREN:
            return "RIGHT_PAREN";
        case COLON:
            return "COLON";
        case SEMICOLON:
            return "SEMICOLON";

        case INTEGER:
            return "INTEGER(" + lexme + ")";
        case IDENTIFIER:
            return "IDENTIFIER(" + lexme + ")";

        case FUNCTION:
            return "FUNCTION";
        case RETURN:
            return "RETURN";

        case NEW_LINE:
            return "NEW_LINE";
        case END:
            return "END";
        case INVALID:
            return "INVALID";
    }
}