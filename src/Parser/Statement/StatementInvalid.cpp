#include "Parser/Statement/StatementInvalid.h"

#include "Lexer/Token.h"

StatementInvalid::StatementInvalid(shared_ptr<Token> token, string message):
Statement(StatementKind::INVALID), token(token), message(message) { }

string StatementInvalid::toString(int indent) {
    return "Error for token " + token->toString() + " at " + to_string(token->getLine()) + ":" + to_string(token->getColumn()) + ": " + message + "\n";
}

string StatementInvalid::getMessage() {
    return message;
}