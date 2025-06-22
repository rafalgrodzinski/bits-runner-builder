#include "Token.h"

vector<TokenKind> Token::tokensEquality = {
    TokenKind::EQUAL,
    TokenKind::NOT_EQUAL
};
vector<TokenKind> Token::tokensComparison = {
    TokenKind::LESS,
    TokenKind::LESS_EQUAL,
    TokenKind::GREATER,
    TokenKind::GREATER_EQUAL
};
vector<TokenKind> Token::tokensTerm = {
    TokenKind::PLUS,
    TokenKind::MINUS
};
vector<TokenKind> Token::tokensFactor = {
    TokenKind::STAR,
    TokenKind::SLASH,
    TokenKind::PERCENT
};
vector<TokenKind> Token::tokensBinary = {
    TokenKind::EQUAL,
    TokenKind::NOT_EQUAL,

    TokenKind::LESS,
    TokenKind::LESS_EQUAL,
    TokenKind::GREATER,
    TokenKind::GREATER_EQUAL,

    TokenKind::PLUS,
    TokenKind::MINUS,
    
    TokenKind::STAR,
    TokenKind::SLASH,
    TokenKind::PERCENT
};
vector<TokenKind> Token::tokensLiteral = {
    TokenKind::BOOL,
    TokenKind::INTEGER_DEC,
    TokenKind::INTEGER_HEX,
    TokenKind::INTEGER_BIN,
    TokenKind::REAL
};

Token::Token(TokenKind kind, string lexme, int line, int column):
    kind(kind), lexme(lexme), line(line), column(column) {
}

TokenKind Token::getKind() {
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
    return kind != TokenKind::INVALID;
}

bool Token::isOfKind(vector<TokenKind> kinds) {
    for (TokenKind &kind : kinds) {
        if (kind == this->kind)
            return true;
    }
        
    return false;
}

string Token::toString() {
    switch (kind) {
        case TokenKind::PLUS:
            return "+";
        case TokenKind::MINUS:
            return "-";
        case TokenKind::STAR:
            return "*";
        case TokenKind::SLASH:
            return "/";
        case TokenKind::PERCENT:
            return "%";
        
        case TokenKind::EQUAL:
            return "=";
        case TokenKind::NOT_EQUAL:
            return "≠";
        case TokenKind::LESS:
            return "<";
        case TokenKind::LESS_EQUAL:
            return "≤";
        case TokenKind::GREATER:
            return ">";
        case TokenKind::GREATER_EQUAL:
            return "≥";

        case TokenKind::LEFT_PAREN:
            return "(";
        case TokenKind::RIGHT_PAREN:
            return ")";
        case TokenKind::COMMA:
            return ",";
        case TokenKind::COLON:
            return ":";
        case TokenKind::SEMICOLON:
            return ";";
        case TokenKind::QUESTION_QUESTION:
            return "??";
        case TokenKind::QUESTION:
            return "?";
        case TokenKind::LEFT_ARROW:
            return "←";
        case TokenKind::RIGHT_ARROW:
            return "→";

        case TokenKind::BOOL:
            return "BOOL(" + lexme + ")";
        case TokenKind::INTEGER_DEC:
            return "INTEGER_DEC(" + lexme + ")";
        case TokenKind::INTEGER_HEX:
            return "INTEGER_HEX(" + lexme + ")";
        case TokenKind::INTEGER_BIN:
            return "INTEGER_BIN(" + lexme + ")";
        case TokenKind::REAL:
            return "REAL(" + lexme + ")";
        case TokenKind::IDENTIFIER:
            return "IDENTIFIER(" + lexme + ")";
        case TokenKind::TYPE:
            return "TYPE(" + lexme + ")";

        case TokenKind::FUNCTION:
            return "FUNCTION";
        case TokenKind::RETURN:
            return "RETURN";

        case TokenKind::M_EXTERN:
            return "@EXTERN";

        case TokenKind::NEW_LINE:
            return "↲";
        case TokenKind::END:
            return "END";
        case TokenKind::INVALID:
            return "INVALID";
    }
}