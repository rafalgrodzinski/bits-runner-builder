#ifndef LEXER_H
#define LEXER_H

#include <memory>
#include <string>
#include <vector>

class Error;
class Token;
enum class TokenKind;

using namespace std;

class Lexer {
private:
    string source;
    int currentIndex;
    string currentFileName;
    int currentLine;
    int currentColumn;
    vector<shared_ptr<Token>> tokens;
    vector<shared_ptr<Error>> errors;
    bool foundRawSourceStart;
    bool isParsingRawSource;

    shared_ptr<Token> nextToken();
    shared_ptr<Token> match(TokenKind kind, const string &lexme, bool needsSeparator);
    shared_ptr<Token> matchFloat();
    shared_ptr<Token> matchIntegerDec();
    shared_ptr<Token> matchIntegerHex();
    shared_ptr<Token> matchIntegerBin();
    shared_ptr<Token> matchIntegerChar();
    shared_ptr<Token> matchString();
    shared_ptr<Token> matchType();
    shared_ptr<Token> matchIdentifier();
    void tryStartingRawSourceParsing();
    shared_ptr<Token> matchRawSourceLine();
    shared_ptr<Token> matchEnd();

    bool isWhiteSpace(int index) const;
    bool isDecDigit(int index) const;
    bool isHexDigit(int index) const;
    bool isBinDigit(int index) const;
    bool isIdentifier(int index) const;
    bool isSeparator(int index) const;
    void advanceWithToken(shared_ptr<Token> token);

    void markError();

public:
    Lexer(const string &fileName, const string &source);
    vector<shared_ptr<Token>> getTokens();
};

#endif