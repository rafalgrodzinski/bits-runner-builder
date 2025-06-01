#include "Lexer.h"

Lexer::Lexer(string source): source(source) {
}

vector<Token> Lexer::getTokens() {
    vector<Token> tokens;
    do {
        Token token = nextToken();

        // Abort scanning if we got an error
        if (!token.isValid()) {
            cerr << "Unexpected character '" << token.getLexme() << "' at " << token.getLine() << ":" << token.getColumn() << endl;
            return vector<Token>();
         }
        
        currentIndex += token.getLexme().length();
        currentColumn += token.getLexme().length();

        if (token.getKind() == Token::Kind::NEW_LINE) {
            currentLine++;
            currentColumn = 0;
        }
        
        // filter out multiple new lines
        if (tokens.empty() || token.getKind() != Token::Kind::NEW_LINE || tokens.back().getKind() != token.getKind())
            tokens.push_back(token);
    } while (tokens.back().getKind() != Token::Kind::END);
    return tokens;
}

Token Lexer::nextToken() {
    while (currentIndex < source.length() && isWhiteSpace(currentIndex)) {
        currentIndex++;
        currentColumn++;
    }

    {
        Token token = matchEnd();
        if (token.isValid())
            return token;
    }

    {
        Token token = matchSymbol('+', Token::Kind::PLUS);
        if (token.isValid())
            return token;
    }

    {
        Token token = matchSymbol('-', Token::Kind::MINUS);
        if (token.isValid())
            return token;
    }

    {
        Token token = matchSymbol('*', Token::Kind::STAR);
        if (token.isValid())
            return token;
    }

    {
        Token token = matchSymbol('/', Token::Kind::SLASH);
        if (token.isValid())
            return token;
    }

    {
        Token token =matchSymbol('%', Token::Kind::PERCENT);
        if (token.isValid())
            return token;
    }

    {
        Token token = matchSymbol('(', Token::Kind::LEFT_PAREN);
        if (token.isValid())
            return token;
    }

    {
        Token token = matchSymbol(')', Token::Kind::RIGHT_PAREN);
        if (token.isValid())
            return token;
    }

    {
        Token token = matchSymbol(':', Token::Kind::COLON);
        if (token.isValid())
            return token;
    }

    {
        Token token = matchSymbol(';', Token::Kind::SEMICOLON);
        if (token.isValid())
            return token;
    }

    {
        Token token = matchKeyword("fun", Token::Kind::FUNCTION);
        if (token.isValid())
            return token;
    }

    {
        Token token = matchKeyword("ret", Token::Kind::RETURN);
        if (token.isValid())
            return token;
    }

    {
        Token token = matchInteger();
        if (token.isValid())
            return token;
    }

    {
        Token token = matchIdentifier();
        if (token.isValid())
            return token;
    }

    {
        Token token = matchNewLine();
        if (token.isValid())
            return token;
    }

    return matchInvalid();
}

Token Lexer::matchEnd() {
    if (currentIndex >= source.length())
        return Token(Token::Kind::END, "", currentLine, currentColumn);
    
    return Token(Token::Kind::INVALID, source.substr(currentIndex, 1), currentLine, currentColumn);
}

Token Lexer::matchNewLine() {
    if (isNewLine(currentIndex))
        return Token(Token::Kind::NEW_LINE, "\n", currentLine, currentColumn);

    return Token(Token::Kind::INVALID, source.substr(currentIndex, 1), currentLine, currentColumn);
}

Token Lexer::matchSymbol(char symbol, Token::Kind kind) {
    if (source.at(currentIndex) == symbol)
        return Token(kind, string(1, symbol), currentLine, currentColumn);

    return Token(Token::Kind::INVALID, source.substr(currentIndex, 1), currentLine, currentColumn);
}

Token Lexer::matchKeyword(string keyword, Token::Kind kind) {
    bool isMatching = source.compare(currentIndex, keyword.length(), keyword) == 0;

    if (isMatching && isSeparator(currentIndex + keyword.length()))
        return Token(kind, keyword, currentLine, currentColumn);
    else
        return Token(Token::Kind::INVALID, source.substr(currentIndex, 1), currentLine, currentColumn);
}

Token Lexer::matchInteger() {
    int nextIndex = currentIndex;

    while (nextIndex < source.length() && isDigit(nextIndex))
        nextIndex++;
    
    if (nextIndex == currentIndex || !isSeparator(nextIndex))
        return Token(Token::Kind::INVALID, source.substr(currentIndex, 1), currentLine, currentColumn);
    
    string lexme = source.substr(currentIndex, nextIndex - currentIndex);
    return Token(Token::Kind::INTEGER, lexme, currentLine, currentColumn);
}

Token Lexer::matchIdentifier() {
    int nextIndex = currentIndex;

    while (nextIndex < source.length() && isIdentifier(nextIndex))
        nextIndex++;

    if (nextIndex == currentIndex || !isSeparator(nextIndex))
        return Token(Token::Kind::INVALID, source.substr(currentIndex, 1), currentLine, currentColumn);

    string lexme = source.substr(currentIndex, nextIndex - currentIndex);
    return Token(Token::Kind::IDENTIFIER, lexme, currentLine, currentColumn);
}

Token Lexer::matchInvalid() {
    return Token(Token::Kind::INVALID, source.substr(currentIndex, 1), currentLine, currentColumn);
}

bool Lexer::isWhiteSpace(int index) {
    char character = source.at(index);
    return character == ' ' || character == '\t';
}

bool Lexer::isNewLine(int index) {
    char character = source.at(index);
    return character == '\n';
}

bool Lexer::isDigit(int index) {
    char character = source.at(index);
    return character >= '0' && character <= '9';
}

bool Lexer::isIdentifier(int index) {
    char character = source.at(index);
    bool isDigit = character >= '0' && character <= '9';
    bool isAlpha = character >= 'a' && character <= 'z' || character >= 'A' && character <= 'Z';
    bool isAlowedSymbol = character == '_';

    return isDigit || isAlpha || isAlowedSymbol;
}

bool Lexer::isSeparator(int index) {
    if (index >= source.length())
        return true;

    char character = source.at(index);
    switch (character) {
        case '+':
        case '-':
        case '*':
        case '/':
        case '%':
        case '(':
        case ')':
        case ':':
        case ' ':
        case '\t':
        case '\n':
            return true;
        default:
            return false;
    }
}
