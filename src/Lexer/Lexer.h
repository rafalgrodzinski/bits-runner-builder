#ifndef LEXER_H
#define LEXER_H

#include <memory>
#include <string>
#include <vector>

class Error;
class Token;
enum class TokenKind;

class Lexer {
public:
    Lexer(const std::string &fileName, const std::string &source);

    std::vector<std::shared_ptr<Token>> getTokens();

private:
    std::string source;
    int currentIndex;
    std::string currentFileName;
    int currentLine;
    int currentColumn;
    std::vector<std::shared_ptr<Token>> tokens;
    std::vector<std::shared_ptr<Error>> errors;
    bool foundRawSourceStart;
    bool isParsingRawSource;

    std::shared_ptr<Token> nextToken();
    std::shared_ptr<Token> match(TokenKind kind, const std::string &lexme, bool needsSeparator);
    std::shared_ptr<Token> matchFloat();
    std::shared_ptr<Token> matchIntegerDec();
    std::shared_ptr<Token> matchIntegerHex();
    std::shared_ptr<Token> matchIntegerBin();
    std::shared_ptr<Token> matchIntegerChar();
    std::shared_ptr<Token> matchString();
    std::shared_ptr<Token> matchType();
    std::shared_ptr<Token> matchIdentifier();
    void tryStartingRawSourceParsing();
    std::shared_ptr<Token> matchRawSourceLine();
    std::shared_ptr<Token> matchEnd();

    bool isWhiteSpace(int index) const;
    bool isDecDigit(int index) const;
    bool isHexDigit(int index) const;
    bool isBinDigit(int index) const;
    bool isIdentifier(int index) const;
    bool isSeparator(int index) const;
    void advanceWithToken(std::shared_ptr<Token> token);

    void markError();
};

#endif