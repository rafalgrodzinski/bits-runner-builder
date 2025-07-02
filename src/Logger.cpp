#include "Logger.h"

#include <iostream>

#include "Lexer/Token.h"

string Logger::toString(shared_ptr<Token> token) {
    switch (token->getKind()) {
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
        case TokenKind::LEFT_ARROW:
            return "←";
        case TokenKind::RIGHT_ARROW:
            return "→";

        case TokenKind::BOOL:
            return "BOOL(" + token->getLexme() + ")";
        case TokenKind::INTEGER_DEC:
            return "INT_DEC(" + token->getLexme() + ")";
        case TokenKind::INTEGER_HEX:
            return "INT_HEX(" + token->getLexme() + ")";
        case TokenKind::INTEGER_BIN:
            return "INT_BIN(" + token->getLexme() + ")";
        case TokenKind::REAL:
            return "REAL(" + token->getLexme() + ")";
        case TokenKind::IDENTIFIER:
            return "ID(" + token->getLexme() + ")";
        case TokenKind::TYPE:
            return "TYPE(" + token->getLexme() + ")";

        case TokenKind::IF:
            return "IF";
        case TokenKind::ELSE:
            return "ELSE";
        case TokenKind::FUNCTION:
            return "FUN";
        case TokenKind::RETURN:
            return "RET";
        case TokenKind::REPEAT:
            return "REP";

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

void Logger::print(vector<shared_ptr<Token>> tokens) {
        for (int i=0; i<tokens.size(); i++) {
            cout << i << "|" << toString(tokens.at(i));
            if (i < tokens.size() - 1)
                cout << "  ";
        }
        cout << endl << endl;
}