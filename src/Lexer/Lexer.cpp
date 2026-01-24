#include "Lexer.h"

#include "Error.h"
#include "Logger.h"
#include "Location.h"
#include "Token.h"

Lexer::Lexer(string fileName, string source):
currentFileName(fileName), source(source) { }

vector<shared_ptr<Token>> Lexer::getTokens() {
    currentIndex = 0;
    currentLine = 0;
    currentColumn = 0;
    foundRawSourceStart = false;
    isParsingRawSource = false;

    tokens.clear();
    errors.clear();
    
    shared_ptr<Token> token;
    do {
        if (token = nextToken()) {
            // Don't add new line as the first token
            if (tokens.empty() && token->isOfKind({TokenKind::NEW_LINE}))
                continue;
            
            // Insert an additional new line just before end
            if (token->getKind() == TokenKind::END && !tokens.empty() && tokens.back()->getKind() != TokenKind::NEW_LINE)
                tokens.push_back(make_shared<Token>(TokenKind::NEW_LINE, "\n", token->getLocation()));

            // filter out multiple new lines
            if (tokens.empty() || token->getKind() != TokenKind::NEW_LINE || tokens.back()->getKind() != token->getKind())
                tokens.push_back(token);
        }
    } while (token == nullptr || token->getKind() != TokenKind::END);

    if (tokens.size() <= 1)
        markError();

    if (!errors.empty()) {
        for (shared_ptr<Error> &error : errors)
            Logger::print(error);
        exit(1);
    }

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
    token = match(TokenKind::END, "//", false); // dummy token kind
    if (token) {
        do {
            // new line
            if (token = match(TokenKind::NEW_LINE, "\n", false))
                return token;
    
            // eof
            if (token = matchEnd())
                return token;

            // if either not found, go to then next character
            currentIndex++;
        } while(true);
    }

    // ignore /* */ comment
    token = match(TokenKind::END, "/*", false); // dummy token kind
    if (token) {
        shared_ptr<Token> newLineToken = nullptr; // we want to return the first new line we come accross
        int depth = 1; // so we can embed comments inside each other
        do {
            // new line
            token = match(TokenKind::NEW_LINE, "\n", false);
            newLineToken = newLineToken ? newLineToken : token;
            if (token) {
                continue;
            }

            // eof
            token = matchEnd();
            if (token) {
                markError();
                return token;
            }

            // go deeper
            token = match(TokenKind::END, "/*", false); // dummy token kind
            if (token) {
                depth++;
                continue;
            }

            // go back
            token = match(TokenKind::END, "*/", false); // dummy token kind
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

    // raw source
    if (token = matchRawSourceLine())
        return token;

    // structural
    if (token = match(TokenKind::LEFT_ROUND_BRACKET, "(", false))
        return token;

    if (token = match(TokenKind::RIGHT_ROUND_BRACKET, ")", false))
        return token;

    if (token = match(TokenKind::LEFT_SQUARE_BRACKET, "[", false))
        return token;

    if (token = match(TokenKind::RIGHT_SQUARE_BRACKET, "]", false))
        return token;

    if (token = match(TokenKind::LEFT_CURLY_BRACKET, "{", false))
        return token;

    if (token = match(TokenKind::RIGHT_CURLY_BRACKET, "}", false))
        return token;

    if (token = match(TokenKind::COMMA, ",", false))
        return token;

    if (token = match(TokenKind::COLON, ":", false))
        return token;

    if (token = match(TokenKind::SEMICOLON, ";", false))
        return token;

    if (token = match(TokenKind::LEFT_ARROW, "<-", false))
         return token;
        
    if (token = match(TokenKind::RIGHT_ARROW, "->", false))
        return token;

    if (token = match(TokenKind::DOT, ".", false))
        return token;

    // logical
    if (token = match(TokenKind::OR, "or", true))
        return token;

    if (token = match(TokenKind::XOR, "xor", true))
        return token;

    if (token = match(TokenKind::AND, "and", true))
        return token;

    if (token = match(TokenKind::NOT, "not", true))
        return token;

    // bitwise
    if (token = match(TokenKind::BIT_TEST, "&?", false))
        return token;

    if (token = match(TokenKind::BIT_OR, "|", false))
        return token;

    if (token = match(TokenKind::BIT_XOR, "^", false))
        return token;

    if (token = match(TokenKind::BIT_AND, "&", false))
        return token;

    if (token = match(TokenKind::BIT_NOT, "~", false))
        return token;

    // comparison
    if (token = match(TokenKind::NOT_EQUAL, "!=", false))
        return token;
    
    if (token = match(TokenKind::EQUAL, "=", false))
        return token;
    
    if (token = match(TokenKind::LESS_EQUAL, "<=", false))
        return token;

    if (token = match(TokenKind::GREATER_EQUAL, ">=", false))
        return token;

    // structural or comparison or bitwise
    if (token = match(TokenKind::LEFT_ANGLE_BRACKET, "<", false))
        return token;

    if (token = match(TokenKind::RIGHT_ANGLE_BRACKET, ">", false))
        return token;

    // arithmetic
    if (token = match(TokenKind::PLUS, "+", false))
        return token;
    
    if (token = match(TokenKind::MINUS, "-", false))
        return token;

    if (token = match(TokenKind::STAR, "*", false))
        return token;

    if (token = match(TokenKind::SLASH, "/", false))
        return token;

    if (token = match(TokenKind::PERCENT, "%", false))
        return token;

    // keywords
    if (token = match(TokenKind::FUNCTION, "fun", true))
        return token;

    if (token = match(TokenKind::RAW_FUNCTION, "raw", true)) {
        foundRawSourceStart = true;
        return token;
    }

    if (token = match(TokenKind::DATA, "data", true))
        return token;

    if (token = match(TokenKind::BLOB, "blob", true))
        return token;

    if (token = match(TokenKind::PTR, "ptr", true))
        return token;

    if (token = match(TokenKind::RETURN, "ret", true))
        return token;

    if (token = match(TokenKind::REPEAT, "rep", true))
        return token;

    if (token = match(TokenKind::IF, "if", true))
        return token;

    if (token = match(TokenKind::ELSE, "else", true))
        return token;
    
    // literal
    if (token = match(TokenKind::BOOL, "true", true))
        return token;

    if (token = match(TokenKind::BOOL, "false", true))
        return token;
    
    if (token = matchFloat())
        return token;

    if (token = matchIntegerDec())
        return token;

    if (token = matchIntegerHex())
        return token;

    if (token = matchIntegerBin())
        return token;

    if (token = matchIntegerChar())
        return token;

    if (token = matchString())
        return token;

    // type
    if (token = matchType())
        return token;

    // identifier
    if (token = matchIdentifier())
        return token;

    // meta
    if (token = match(TokenKind::M_MODULE, "@module", true))
        return token;

    if (token = match(TokenKind::M_IMPORT, "@import", true))
        return token;

    if (token = match(TokenKind::M_EXPORT, "@export", true))
        return token;

    if (token = match(TokenKind::M_EXTERN, "@extern", true))
        return token;

    if (token = match(TokenKind::META, "@", false))
        return token;

    // new line
    if (token = match(TokenKind::NEW_LINE, "\n", false)) {
        tryStartingRawSourceParsing();
        return token;
    }
    
    // eof
    if (token = matchEnd())
        return token;

    markError();
    return nullptr;
}

shared_ptr<Token> Lexer::match(TokenKind kind, string lexme, bool needsSeparator) {
    if (currentIndex + lexme.length() > source.length())
        return nullptr;

    bool isMatching = source.compare(currentIndex, lexme.length(), lexme) == 0;
    bool isSeparatorSatisfied = !needsSeparator || isSeparator(currentIndex + lexme.length());

    if (!isMatching || !isSeparatorSatisfied)
        return nullptr;

    shared_ptr<Token> token = make_shared<Token>(kind, lexme, make_shared<Location>(currentFileName, currentLine, currentColumn));
    advanceWithToken(token);
    return token;
}

shared_ptr<Token> Lexer::matchFloat() {
    int nextIndex = currentIndex;

    // Match digit or _ if it's not in the first position
    while (nextIndex < source.length() && (isDecDigit(nextIndex) || (source.at(nextIndex) == '_' && nextIndex > currentIndex)))
        nextIndex++;
    
    // Last character should be . and it shouldn't be preceeded by _
    if (nextIndex >= source.length() || source.at(nextIndex) != '.' || source.at(nextIndex-1) == '_')
        return nullptr;
    else
        nextIndex++;

    int fractionalIndex = nextIndex;
    
    // Match digit or _ if it's not in the first position
    while (nextIndex < source.length() && (isDecDigit(nextIndex) || (source.at(nextIndex) == '_' && nextIndex > fractionalIndex)))
        nextIndex++;

    // Next symbol should be separator and the last symbol shouldn't be _ or .
    if (nextIndex == fractionalIndex || !isSeparator(nextIndex) || source.at(nextIndex-1) == '_') {
        return nullptr;
    }

    string lexme = source.substr(currentIndex, nextIndex - currentIndex);
    shared_ptr<Token> token = make_shared<Token>(TokenKind::FLOAT, lexme, make_shared<Location>(currentFileName, currentLine, currentColumn));
    advanceWithToken(token);
    return token;
}

shared_ptr<Token> Lexer::matchIntegerDec() {
    int nextIndex = currentIndex;

    // Match digit or _ if it's not in the first position
    while (nextIndex < source.length() && (isDecDigit(nextIndex) || (source.at(nextIndex) == '_' && nextIndex > currentIndex)))
        nextIndex++;
    
    // Resulting number shouldn't be empty, should be separated on the right, and _ shouldn't be the last character
    if (nextIndex == currentIndex || !isSeparator(nextIndex) || source.at(nextIndex-1) == '_')
        return nullptr;
    
    string lexme = source.substr(currentIndex, nextIndex - currentIndex);
    shared_ptr<Token> token = make_shared<Token>(TokenKind::INTEGER_DEC, lexme, make_shared<Location>(currentFileName, currentLine, currentColumn));
    advanceWithToken(token);
    return token;
}

shared_ptr<Token> Lexer::matchIntegerHex() {
    int nextIndex = currentIndex;

    // match 0x
    if (nextIndex > int(source.length()) - 2)
        return nullptr;

    if (source.at(nextIndex++) != '0' || source.at(nextIndex++) != 'x')
        return nullptr;

    // Include _ which is not on the first position
    while (nextIndex < source.length() && (isHexDigit(nextIndex) || (nextIndex > currentIndex+2 && source.at(nextIndex) == '_')))
        nextIndex++;

    // Resulting number shouldn't be empty, should be separated on the right, and _ shouldn't be the last character
    if (nextIndex == currentIndex+2 || !isSeparator(nextIndex) || source.at(nextIndex-1) == '_')
        return nullptr;
    
    string lexme = source.substr(currentIndex, nextIndex - currentIndex);
    shared_ptr<Token> token = make_shared<Token>(TokenKind::INTEGER_HEX, lexme, make_shared<Location>(currentFileName, currentLine, currentColumn));
    advanceWithToken(token);
    return token;
}

shared_ptr<Token> Lexer::matchIntegerBin() {
    int nextIndex = currentIndex;

    // match 0b
    if (nextIndex > int(source.length()) - 2)
        return nullptr;

    if (source.at(nextIndex++) != '0' || source.at(nextIndex++) != 'b')
        return nullptr;

    // Include _ which is not on the first position    
    while (nextIndex < source.length() && (isBinDigit(nextIndex) || (nextIndex > currentIndex+2 && source.at(nextIndex) == '_')))
        nextIndex++;
    
    // Resulting number shouldn't be empty, should be separated on the right, and _ shouldn't be the last character
    if (nextIndex == currentIndex || !isSeparator(nextIndex) || source.at(nextIndex-1) == '_')
        return nullptr;
    
    string lexme = source.substr(currentIndex, nextIndex - currentIndex);
    shared_ptr<Token> token = make_shared<Token>(TokenKind::INTEGER_BIN, lexme, make_shared<Location>(currentFileName, currentLine, currentColumn));
    advanceWithToken(token);
    return token;
}

shared_ptr<Token> Lexer::matchIntegerChar() {
    int nextIndex = currentIndex;

    if (currentIndex >= source.size() || source.at(nextIndex) != '\'')
        return nullptr;

    bool isClosing = false;
    do {
        nextIndex++;
        isClosing = (source.at(nextIndex) == '\'' && (source.at(nextIndex - 1) != '\\')) ||
            (nextIndex >= 2 && source.at(nextIndex - 2) == '\\');
    } while (nextIndex < source.length()-1 && !isClosing);

    if (!isClosing)
        return nullptr;

    string lexme = source.substr(currentIndex, nextIndex - currentIndex + 1);
    shared_ptr<Token> token = make_shared<Token>(TokenKind::INTEGER_CHAR, lexme, make_shared<Location>(currentFileName, currentLine, currentColumn));
    advanceWithToken(token);
    return token;
}

shared_ptr<Token> Lexer::matchString() {
    int nextIndex = currentIndex;

    if (currentIndex >= source.size() || source.at(nextIndex) != '\"')
        return nullptr;

    bool isClosing = false;
    bool shouldEscape = false;
    do {
        nextIndex++;
        
        // not escaping characters
        if (!shouldEscape) {
            switch (source.at(nextIndex)) {
                case '\\': // should the next character be escaped?
                    shouldEscape = true;
                    break;
                case '\"': // are closing the string?
                    isClosing = true;
                    break;
            }
        // escape one character
        } else {
            shouldEscape = false;
        }
    } while (nextIndex < source.length() && !isClosing);

    if (!isClosing)
        return nullptr;

    string lexme = source.substr(currentIndex, nextIndex - currentIndex + 1);
    shared_ptr<Token> token = make_shared<Token>(TokenKind::STRING, lexme, make_shared<Location>(currentFileName, currentLine, currentColumn));
    advanceWithToken(token);
    return token;
}

shared_ptr<Token> Lexer::matchType() {
    int nextIndex = currentIndex;

    if (tokens.empty() || !tokens.back()->isOfKind({TokenKind::IDENTIFIER, TokenKind::LEFT_ANGLE_BRACKET, TokenKind::RIGHT_ARROW}))
        return nullptr;

    // TYPE < TYPE [..]
    if (tokens.size() >= 2 && tokens.back()->isOfKind({TokenKind::LEFT_ANGLE_BRACKET}) && !tokens.at(tokens.size() - 2)->isOfKind({TokenKind::TYPE}))
        return nullptr;

    while (nextIndex < source.length() && isIdentifier(nextIndex))
        nextIndex++;

    if (nextIndex == currentIndex || !isSeparator(nextIndex))
        return nullptr;

    string lexme = source.substr(currentIndex, nextIndex - currentIndex);
    shared_ptr<Token> token = make_shared<Token>(TokenKind::TYPE, lexme, make_shared<Location>(currentFileName, currentLine, currentColumn));
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

    // Special case for misplaced type tokens
    if (
        lexme.compare("bool") == 0
        || lexme.compare("u8") == 0 || lexme.compare("u16") == 0 || lexme.compare("u32") == 0 || lexme.compare("u64") == 0
        || lexme.compare("s8") == 0 || lexme.compare("s16") == 0 || lexme.compare("s32") == 0 || lexme.compare("s64") == 0
        || lexme.compare("f32") == 0 || lexme.compare("f64") == 0
        || lexme.compare("a") == 0 || lexme.compare("ptr") == 0
        || lexme.compare("data") == 0 || lexme.compare("blob") == 0
    ){
        shared_ptr<Token> token = make_shared<Token>(TokenKind::TYPE, lexme, make_shared<Location>(currentFileName, currentLine, currentColumn));
        advanceWithToken(token);
        return token;
    }

    shared_ptr<Token> token = make_shared<Token>(TokenKind::IDENTIFIER, lexme, make_shared<Location>(currentFileName, currentLine, currentColumn));
    advanceWithToken(token);
    return token;
}

void Lexer::tryStartingRawSourceParsing() {
    if (!foundRawSourceStart)
        return;

    if (!tokens.at(tokens.size() - 1)->isOfKind({TokenKind::COLON, TokenKind::COMMA, TokenKind::RIGHT_ARROW})) {
        foundRawSourceStart = false;
        isParsingRawSource = true;
    }
}

shared_ptr<Token> Lexer::matchRawSourceLine() {
    int nextIndex = currentIndex;

    if (!isParsingRawSource)
        return nullptr;

    if (source.at(nextIndex) == ';') {
        isParsingRawSource = false;
        return nullptr;
    }

    while (source.at(nextIndex) != '\n')
        nextIndex++;

    string lexme = source.substr(currentIndex, nextIndex - currentIndex);
    shared_ptr<Token> token =  make_shared<Token>(TokenKind::RAW_SOURCE_LINE, lexme, make_shared<Location>(currentFileName, currentLine, currentColumn));
    advanceWithToken(token);
    currentIndex++; // skip newline
    return token;
}

shared_ptr<Token> Lexer::matchEnd() {
    if (currentIndex >= source.length())
        return make_shared<Token>(TokenKind::END, "", make_shared<Location>(currentFileName, currentLine, currentColumn));
    
    return nullptr;
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

bool Lexer::isBinDigit(int index) {
    char character = source.at(index);
    return character == '0' || character == '1';
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
        case '[':
        case ']':
        case '{':
        case '}':
        case ',':
        case ':':
        case ';':
        case '|':
        case '^':
        case '&':
        case '~':
        case ' ':
        case '\t':
        case '\n':
        case '.':
            return true;
        default:
            return false;
    }
}

void Lexer::advanceWithToken(shared_ptr<Token> token) {
    switch (token->getKind()) {
        case TokenKind::NEW_LINE:
        case TokenKind::RAW_SOURCE_LINE:
            currentLine++;
            currentColumn = 0;
            break;
        default:
            currentColumn += token->getLexme().length();
            break;
    }
    currentIndex += token->getLexme().length();
}

void Lexer::markError() {
    int startIndex = currentIndex;
    int startColumn = currentColumn;
    string lexme;
    if (currentIndex < source.length()) {
        do {
            currentIndex++;
            currentColumn++;
        } while (!isSeparator(currentIndex));
        lexme = source.substr(startIndex, currentIndex - startIndex);
    } else {
        lexme = "EOF";
    }
    errors.push_back(
        Error::lexerError(
            make_shared<Location>(currentFileName, currentLine, currentColumn),
            lexme
        )
    );
}
