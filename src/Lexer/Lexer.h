#ifndef LEXER_H
#define LEXER_H

#include <vector>

class Token;
enum class TokenKind;
class Error;

using namespace std;

class Lexer {
private:
    string source;
    int currentIndex;
    int currentLine;
    int currentColumn;
    vector<shared_ptr<Error>> errors;

    shared_ptr<Token> nextToken();
    shared_ptr<Token> match(TokenKind kind, string lexme, bool needsSeparator);
    shared_ptr<Token> matchIntegerDec();
    shared_ptr<Token> matchIntegerHex();
    shared_ptr<Token> matchIntegerBin();
    shared_ptr<Token> matchReal();
    shared_ptr<Token> matchIdentifier();
    shared_ptr<Token> matchEnd();

    bool isWhiteSpace(int index);
    bool isDecDigit(int index);
    bool isHexDigit(int index);
    bool isBinDigit(int index);
    bool isIdentifier(int index);
    bool isSeparator(int index);
    void advanceWithToken(shared_ptr<Token> token);

    void markError();

public:
    Lexer(string source);
    vector<shared_ptr<Token>> getTokens();
};

#endif