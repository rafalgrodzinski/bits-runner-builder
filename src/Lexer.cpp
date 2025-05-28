#include "Lexer.h"

Lexer::Lexer(std::string source) : source(source) {
}

std::vector<Token> Lexer::getTokens() {
    std::vector<Token> tokens;
    do {
        Token token = nextToken();
        currentIndex += token.getLexme().length();

        if (token.getKind() == Token::Kind::NEW_LINE)
            currentLine++;
        
        // filter out multiple new lines
        if (tokens.empty() || token.getKind() != Token::Kind::NEW_LINE || tokens.back() != token)
            tokens.push_back(token);
    } while (tokens.back().getKind() != Token::Kind::END);
    return tokens;
}

Token Lexer::nextToken() {
    Token token = Token::Invalid;

    while (currentIndex < source.length() && isWhiteSpace(currentIndex))
        currentIndex++;

    do {
        if ((token = matchEnd()) != Token::Invalid)
            break;
    
        if ((token = matchSymbol('+', Token::Kind::PLUS)) != Token::Invalid)
            break;
        
        if ((token = matchSymbol('-', Token::Kind::MINUS)) != Token::Invalid)
            break;

        if ((token = matchSymbol('*', Token::Kind::STAR)) != Token::Invalid)
            break;

        if ((token = matchSymbol('/', Token::Kind::SLASH)) != Token::Invalid)
            break;

        if ((token = matchSymbol('%', Token::Kind::PERCENT)) != Token::Invalid)
            break;

        if ((token = matchSymbol('(', Token::Kind::LEFT_PAREN)) != Token::Invalid)
            break;

        if ((token = matchSymbol(')', Token::Kind::RIGHT_PAREN)) != Token::Invalid)
            break;

        if ((token = matchSymbol('.', Token::Kind::DOT)) != Token::Invalid)
            break;

        if ((token = matchSymbol(',', Token::Kind::COMMA)) != Token::Invalid)
            break;

        if ((token = matchInteger()) != Token::Invalid)
            break;

        if ((token = matchNewLine()) != Token::Invalid)
            break;
        
        token = matchInvalid();
    } while(false);

    return token;
}

Token Lexer::matchEnd() {
    if (currentIndex >= source.length())
        return Token(Token::Kind::END, "");
    
    return Token::Invalid;
}

Token Lexer::matchNewLine() {
    if (isNewLine(currentIndex))
        return Token(Token::Kind::NEW_LINE, "\n");

    return Token::Invalid;
}

Token Lexer::matchSymbol(char symbol, Token::Kind kind) {
    if (source.at(currentIndex) == symbol)
        return Token(kind, std::string(1, symbol));

    return Token::Invalid;
}

Token Lexer::matchInteger() {
    int nextIndex = currentIndex;

    while (nextIndex < source.length() && isDigit(nextIndex))
        nextIndex++;
    
    if (nextIndex == currentIndex)
        return Token::Invalid;
    
    std::string lexme = source.substr(currentIndex, nextIndex - currentIndex);
    return Token(Token::Kind::INTEGER, lexme);
}

Token Lexer::matchInvalid() {
    char symbol = source.at(currentIndex);
    return Token(Token::Kind::INVALID, std::string(1, symbol));
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
