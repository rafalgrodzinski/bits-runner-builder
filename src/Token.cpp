#include "Token.h"

vector<Token::Kind> Token::tokensEquality = {
    Token::Kind::EQUAL,
    Token::Kind::NOT_EQUAL
};
vector<Token::Kind> Token::tokensComparison = {
    Token::Kind::LESS,
    Token::Kind::LESS_EQUAL,
    Token::Kind::GREATER,
    Token::Kind::GREATER_EQUAL
};
vector<Token::Kind> Token::tokensTerm = {
    Token::Kind::PLUS,
    Token::Kind::MINUS
};
vector<Token::Kind> Token::tokensFactor = {
    Token::Kind::STAR,
    Token::Kind::SLASH,
    Token::Kind::PERCENT
};
vector<Token::Kind> Token::tokensBinary = {
    Token::Kind::EQUAL,
    Token::Kind::NOT_EQUAL,

    Token::Kind::LESS,
    Token::Kind::LESS_EQUAL,
    Token::Kind::GREATER,
    Token::Kind::GREATER_EQUAL,

    Token::Kind::PLUS,
    Token::Kind::MINUS,
    
    Token::Kind::STAR,
    Token::Kind::SLASH,
    Token::Kind::PERCENT
};

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
        case QUESTION_QUESTION:
            return "QUESTION_QUESTION";
        case QUESTION:
            return "QUESTION";

        case BOOL:
            return "BOOL(" + lexme + ")";
        case INTEGER:
            return "INTEGER(" + lexme + ")";
        case REAL:
            return "REAL(" + lexme + ")";
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