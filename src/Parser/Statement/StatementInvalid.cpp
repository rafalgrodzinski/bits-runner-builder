#include "Parser/Statement/StatementInvalid.h"

#include "Lexer/Token.h"

StatementInvalid::StatementInvalid(shared_ptr<Token> token, string message):
Statement(StatementKind::INVALID), token(token), message(message) { }

string StatementInvalid::getMessage() {
    return message;
}