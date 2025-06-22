#include "Lexer.h"

Lexer::Lexer(string source): source(source) {
}

vector<shared_ptr<Token>> Lexer::getTokens() {
    shared_ptr<Token> token = nullptr;
    tokens.clear();
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
        
        // Insert an additional new line just before end
        if (token->getKind() == TokenKind::END && tokens.back()->getKind() != TokenKind::NEW_LINE)
            tokens.push_back(make_shared<Token>(TokenKind::NEW_LINE, "\n", token->getLine(), token->getColumn()));

        // filter out multiple new lines
        if (tokens.empty() || token->getKind() != TokenKind::NEW_LINE || tokens.back()->getKind() != token->getKind())
            tokens.push_back(token);
    } while (token->getKind() != TokenKind::END);
    return tokens;
}

shared_ptr<Token> Lexer::nextToken() {
    // Ignore white spaces
    while (currentIndex < source.length() && isWhiteSpace(currentIndex)) {
        currentIndex++;
        currentColumn++;
    }

    shared_ptr<Token> token;

    // ignore // comment
    token = match(TokenKind::INVALID, "//", false);
    if (token) {
        currentIndex += 2;
        do {
            // new line
            token = match(TokenKind::NEW_LINE, "\n", false);
            if (token != nullptr)
                return token;
    
            // eof
            token = matchEnd();
            if (token != nullptr)
                return token;

            // if either not found, go to then next character
            currentIndex++;
        } while(true);
    }

    // ignore /* */ comment
    token = match(TokenKind::INVALID, "/*", false);
    if (token) {
        shared_ptr<Token> newLineToken = nullptr; // we want to return the first new line we come accross
        int depth = 1; // so we can embed comments inside each other
        do {
            // new line
            token = match(TokenKind::NEW_LINE, "\n", false);
            newLineToken = newLineToken ? newLineToken : token;
            if (token) {
                continue;;
            }

            // eof
            token = matchEnd();
            if (token)
                return make_shared<Token>(TokenKind::INVALID, "", currentLine, currentColumn);

            // go deeper
            token = match(TokenKind::INVALID, "/*", false);
            if (token) {
                depth++;
                continue;
            }

            // go back
            token = match(TokenKind::INVALID, "*/", false);
            if (token) {
                depth--;
            }

            if (depth > 0) {
                currentIndex++;
                currentColumn++;
            }
        } while(depth > 0);

        if (newLineToken)
            return newLineToken;
        else
            return nextToken(); // gets rid of remaining white spaces without repeating the code
    }

    // structural
    token = match(TokenKind::LEFT_PAREN, "(", false);
    if (token != nullptr)
        return token;

    token = match(TokenKind::RIGHT_PAREN, ")", false);
    if (token != nullptr)
        return token;

    token = match(TokenKind::COMMA, ",", false);
    if (token != nullptr)
        return token;

    token = match(TokenKind::COLON, ":", false);
    if (token != nullptr)
        return token;

    token = match(TokenKind::SEMICOLON, ";", false);
    if (token != nullptr)
        return token;
    
    token = match(TokenKind::QUESTION_QUESTION, "??", false);
    if (token != nullptr)
        return token;
    
    token = match(TokenKind::QUESTION, "?", false);
    if (token != nullptr)
        return token;

    token = match(TokenKind::LEFT_ARROW, "<-", false);
    if (token != nullptr)
         return token;
        
    token = match(TokenKind::RIGHT_ARROW, "->", false);
    if (token != nullptr)
        return token;

    // arithmetic
    token = match(TokenKind::PLUS, "+", false);
    if (token != nullptr)
        return token;
    
    token = match(TokenKind::MINUS, "-", false);
    if (token != nullptr)
        return token;

    token = match(TokenKind::STAR, "*", false);
    if (token != nullptr)
        return token;

    token = match(TokenKind::SLASH, "/", false);
    if (token != nullptr)
        return token;

    token = match(TokenKind::PERCENT, "%", false);
    if (token != nullptr)
        return token;
    
    // comparison
    token = match(TokenKind::NOT_EQUAL, "!=", false);
    if (token != nullptr)
        return token;
    
    token = match(TokenKind::EQUAL, "=", false);
    if (token != nullptr)
        return token;
    
    token = match(TokenKind::LESS_EQUAL, "<=", false);
    if (token != nullptr)
        return token;

    token = match(TokenKind::LESS, "<", false);
    if (token != nullptr)
        return token;

    token = match(TokenKind::GREATER_EQUAL, ">=", false);
    if (token != nullptr)
        return token;

    token = match(TokenKind::GREATER, ">", false);
    if (token != nullptr)
        return token;

    // keywords
    token = match(TokenKind::FUNCTION, "fun", true);
    if (token != nullptr)
        return token;
    
    token = match(TokenKind::RETURN, "ret", true);
    if (token != nullptr)
        return token;
    
    // literal
    token = match(TokenKind::BOOL, "true", true);
    if (token != nullptr)
        return token;

    token = match(TokenKind::BOOL, "false", true);
    if (token != nullptr)
        return token;
    
    token = matchReal();
    if (token != nullptr)
        return token;

    token = matchIntegerDec();
    if (token != nullptr)
        return token;

    token = matchIntegerHex();
    if (token != nullptr)
        return token;


    // type
    token = match(TokenKind::TYPE, "bool", true);
    if (token != nullptr)
        return token;

    token = match(TokenKind::TYPE, "sint32", true);
    if (token != nullptr)
        return token;

    token = match(TokenKind::TYPE, "real32", true);
    if (token != nullptr)
        return token;

    // identifier
    token = matchIdentifier();
    if (token != nullptr)
        return token;

    // meta
    token = match(TokenKind::M_EXTERN, "@extern", true);
    if (token != nullptr)
        return token;

    // new line
    token = match(TokenKind::NEW_LINE, "\n", false);
    if (token != nullptr)
        return token;
    
    // eof
    token = matchEnd();
    if (token != nullptr)
        return token;

    return matchInvalid();
}

shared_ptr<Token> Lexer::match(TokenKind kind, string lexme, bool needsSeparator) {
    bool isMatching = source.compare(currentIndex, lexme.length(), lexme) == 0;
    bool isSeparatorSatisfied = !needsSeparator || isSeparator(currentIndex + lexme.length());

    if (!isMatching || !isSeparatorSatisfied)
        return nullptr;

    shared_ptr<Token> token = make_shared<Token>(kind, lexme, currentLine, currentColumn);
    advanceWithToken(token);
    return token;
}

shared_ptr<Token> Lexer::matchIntegerDec() {
    int nextIndex = currentIndex;

    while (nextIndex < source.length() && isDecDigit(nextIndex))
        nextIndex++;
    
    if (nextIndex == currentIndex || !isSeparator(nextIndex))
        return nullptr;
    
    string lexme = source.substr(currentIndex, nextIndex - currentIndex);
    shared_ptr<Token> token = make_shared<Token>(TokenKind::INTEGER_DEC, lexme, currentLine, currentColumn);
    advanceWithToken(token);
    return token;
}

shared_ptr<Token> Lexer::matchIntegerHex() {
    int nextIndex = currentIndex;

    // match 0x
    if (nextIndex > source.length()-2)
        return nullptr;

    if (source.at(nextIndex++) != '0' || source.at(nextIndex++) != 'x')
        return nullptr;

    while (nextIndex < source.length() && isHexDigit(nextIndex))
        nextIndex++;
    
    if (nextIndex == currentIndex || !isSeparator(nextIndex))
        return nullptr;
    
    string lexme = source.substr(currentIndex, nextIndex - currentIndex);
    shared_ptr<Token> token = make_shared<Token>(TokenKind::INTEGER_HEX, lexme, currentLine, currentColumn);
    advanceWithToken(token);
    return token;
}

shared_ptr<Token> Lexer::matchReal() {
    int nextIndex = currentIndex;

    while (nextIndex < source.length() && isDecDigit(nextIndex))
        nextIndex++;
    
    if (nextIndex >= source.length() || source.at(nextIndex) != '.')
        return nullptr;
    else
        nextIndex++;
    
    while (nextIndex < source.length() && isDecDigit(nextIndex))
        nextIndex++;

    if (!isSeparator(nextIndex))
        return matchInvalid();

    string lexme = source.substr(currentIndex, nextIndex - currentIndex);
    shared_ptr<Token> token = make_shared<Token>(TokenKind::REAL, lexme, currentLine, currentColumn);
    advanceWithToken(token);
    return token;
}

shared_ptr<Token> Lexer::matchIdentifier() {
    int nextIndex = currentIndex;

    while (nextIndex < source.length() && isIdentifier(nextIndex))
        nextIndex++;

    if (nextIndex == currentIndex || !isSeparator(nextIndex))
        return nullptr;

    string lexme = source.substr(currentIndex, nextIndex - currentIndex);
    shared_ptr<Token> token = make_shared<Token>(TokenKind::IDENTIFIER, lexme, currentLine, currentColumn);
    advanceWithToken(token);
    return token;
}

shared_ptr<Token> Lexer::matchEnd() {
    if (currentIndex >= source.length())
        return make_shared<Token>(TokenKind::END, "", currentLine, currentColumn);
    
    return nullptr;
}

shared_ptr<Token> Lexer::matchInvalid() {
    return make_shared<Token>(TokenKind::INVALID, source.substr(currentIndex, 1), currentLine, currentColumn);
}

bool Lexer::isWhiteSpace(int index) {
    char character = source.at(index);
    return character == ' ' || character == '\t';
}

bool Lexer::isDecDigit(int index) {
    char character = source.at(index);
    return character >= '0' && character <= '9';
}

bool Lexer::isHexDigit(int index) {
    char character = source.at(index);
    return (character >= '0' && character <= '9') || (character >= 'a' && character <= 'f');
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
        case ',':
        case ':':
        case ';':
        case ' ':
        case '\t':
        case '\n':
            return true;
        default:
            return false;
    }
}

 void Lexer::advanceWithToken(shared_ptr<Token> token) {
    if (token->getKind() == TokenKind::NEW_LINE) {
        currentLine++;
        currentColumn = 0;
    } else {
        currentColumn += token->getLexme().length();
    }
    currentIndex += token->getLexme().length();
 }
