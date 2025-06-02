#include "Lexer.h"

Lexer::Lexer(string source): source(source) {
}

vector<shared_ptr<Token>> Lexer::getTokens() {
    vector<shared_ptr<Token>> tokens;
    shared_ptr<Token> token = nullptr;
    do {
        token = nextToken();
        // Got a nullptr, shouldn't have happened
        if (!token) {
            cerr << "Failed to scan tokens" << endl;
            exit(1);
        }

        // Abort scanning if we got an error
        if (!token->isValid()) {
            cerr << "Unexpected character '" << token->getLexme() << "' at " << token->getLine() << ":" << token->getColumn() << endl;
            exit(1);
         }
        
        currentIndex += token->getLexme().length();
        currentColumn += token->getLexme().length();

        if (token->getKind() == Token::Kind::NEW_LINE) {
            currentLine++;
            currentColumn = 0;
        }
        
        // filter out multiple new lines
        if (tokens.empty() || token->getKind() != Token::Kind::NEW_LINE || tokens.back()->getKind() != token->getKind())
            tokens.push_back(token);
    } while (token->getKind() != Token::Kind::END);
    return tokens;
}

shared_ptr<Token> Lexer::nextToken() {    
    while (currentIndex < source.length() && isWhiteSpace(currentIndex)) {
        currentIndex++;
        currentColumn++;
    }

    shared_ptr<Token> token;

    // arithmetic
    token = match(Token::Kind::PLUS, "+", false);
    if (token != nullptr)
        return token;
    
    token = match(Token::Kind::MINUS, "-", false);
    if (token != nullptr)
        return token;

    token = match(Token::Kind::STAR, "*", false);
    if (token != nullptr)
        return token;

    token = match(Token::Kind::SLASH, "/", false);
    if (token != nullptr)
        return token;

    token = match(Token::Kind::PERCENT, "%", false);
    if (token != nullptr)
        return token;
    
    // logical
    token = match(Token::Kind::NOT_EQUAL, "!=", false);
    if (token != nullptr)
        return token;
    
    token = match(Token::Kind::EQUAL, "=", false);
    if (token != nullptr)
        return token;
    
    token = match(Token::Kind::LESS_EQUAL, "<=", false);
    if (token != nullptr)
        return token;

    token = match(Token::Kind::LESS, "<", false);
    if (token != nullptr)
        return token;

    token = match(Token::Kind::GREATER_EQUAL, ">=", false);
    if (token != nullptr)
        return token;

    token = match(Token::Kind::GREATER, ">", false);
    if (token != nullptr)
        return token;

    // structural
    token = match(Token::Kind::LEFT_PAREN, "(", false);
    if (token != nullptr)
        return token;

    token = match(Token::Kind::RIGHT_PAREN, ")", false);
    if (token != nullptr)
        return token;

    token = match(Token::Kind::COLON, ":", false);
    if (token != nullptr)
        return token;

    token = match(Token::Kind::SEMICOLON, ";", false);
    if (token != nullptr)
        return token;

    // keywords
    token = match(Token::Kind::FUNCTION, "fun", true);
    if (token != nullptr)
        return token;
    
    token = match(Token::Kind::RETURN, "ret", true);
    if (token != nullptr)
        return token;
    
    // literal
    token = matchInteger();
    if (token != nullptr)
        return token;

    // identifier
    token = matchIdentifier();
    if (token != nullptr)
        return token;

    // new line
    token = match(Token::Kind::NEW_LINE, "\r\n", false);
    if (token != nullptr)
        return token;

    token = match(Token::Kind::NEW_LINE, "\n", false);
    if (token != nullptr)
        return token;
    
    // other
    token = matchEnd();
    if (token != nullptr)
        return token;

    return matchInvalid();
}

shared_ptr<Token> Lexer::match(Token::Kind kind, string lexme, bool needsSeparator) {
    bool isMatching = source.compare(currentIndex, lexme.length(), lexme) == 0;
    bool isSeparatorSatisfied = !needsSeparator || isSeparator(currentIndex + lexme.length());

    if (isMatching && isSeparatorSatisfied)
        return make_shared<Token>(kind, lexme, currentLine, currentColumn);
    else
        return nullptr;
}

shared_ptr<Token> Lexer::matchInteger() {
    int nextIndex = currentIndex;

    while (nextIndex < source.length() && isDigit(nextIndex))
        nextIndex++;
    
    if (nextIndex == currentIndex || !isSeparator(nextIndex))
        return nullptr;
    
    string lexme = source.substr(currentIndex, nextIndex - currentIndex);
    return make_shared<Token>(Token::Kind::INTEGER, lexme, currentLine, currentColumn);
}

shared_ptr<Token> Lexer::matchIdentifier() {
    int nextIndex = currentIndex;

    while (nextIndex < source.length() && isIdentifier(nextIndex))
        nextIndex++;

    if (nextIndex == currentIndex || !isSeparator(nextIndex))
        return nullptr;

    string lexme = source.substr(currentIndex, nextIndex - currentIndex);
    return make_shared<Token>(Token::Kind::IDENTIFIER, lexme, currentLine, currentColumn);
}

shared_ptr<Token> Lexer::matchEnd() {
    if (currentIndex >= source.length())
        return make_shared<Token>(Token::Kind::END, "", currentLine, currentColumn);
    
    return nullptr;
}

shared_ptr<Token> Lexer::matchInvalid() {
    return make_shared<Token>(Token::Kind::INVALID, source.substr(currentIndex, 1), currentLine, currentColumn);
}

bool Lexer::isWhiteSpace(int index) {
    char character = source.at(index);
    return character == ' ' || character == '\t';
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
        case '=':
        case '<':
        case '>':
        case '(':
        case ')':
        case ':':
        case ';':
        case ' ':
        case '\t':
        case '\n':
        case '\r';
            return true;
        default:
            return false;
    }
}
