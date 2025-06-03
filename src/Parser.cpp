#include "Parser.h"

Parser::Parser(vector<shared_ptr<Token>> tokens): tokens(tokens) {
}

vector<shared_ptr<Statement>> Parser::getStatements() {
    vector<shared_ptr<Statement>> statements;

    while (tokens.at(currentIndex)->getKind() != Token::Kind::END) {
        shared_ptr<Statement> statement = nextStatement();
        // Abort parsing if we got an error
        if (!statement->isValid()) {
            cerr << statement->toString();
            exit(1);
        }
        statements.push_back(statement);
    }

    return statements;
}

//
// Statement
//
shared_ptr<Statement> Parser::nextStatement() {
   shared_ptr<Statement> statement;

    statement = matchStatementFunctionDeclaration();
    if (statement != nullptr)
        return statement;

    statement = matchStatementReturn();
    if (statement != nullptr)
        return statement;

    statement = matchStatementExpression();
    if (statement != nullptr)
        return statement;

    return matchStatementInvalid();
}

shared_ptr<Statement> Parser::matchStatementFunctionDeclaration() {
    if (!matchesTokenKinds({Token::Kind::IDENTIFIER, Token::Kind::COLON, Token::Kind::FUNCTION}))
        return nullptr;

    shared_ptr<Token> identifierToken = tokens.at(currentIndex);
    currentIndex++;
    currentIndex++; // skip colon
    currentIndex++; // skip fun

    while (tokens.at(currentIndex)->getKind() != Token::Kind::NEW_LINE) {
        currentIndex++;
    }
    currentIndex++; // new line
    shared_ptr<Statement> statementBlock = matchStatementBlock();
    if (statementBlock == nullptr)
        return matchStatementInvalid();
    else if (!statementBlock->isValid())
        return statementBlock;
    else
        return make_shared<StatementFunctionDeclaration>(identifierToken->getLexme(), dynamic_pointer_cast<StatementBlock>(statementBlock));
}

shared_ptr<Statement> Parser::matchStatementBlock() {
    vector<shared_ptr<Statement>> statements;

    while (tokens.at(currentIndex)->getKind() != Token::Kind::SEMICOLON) {
        shared_ptr<Statement> statement = nextStatement();
        if (statement == nullptr)
            return matchStatementInvalid();
        else if (!statement->isValid())
            return statement;
        else
            statements.push_back(statement);
    }
    currentIndex++; // skip ;

    if (!tokens.at(currentIndex)->isOfKind({Token::Kind::NEW_LINE, Token::Kind::END}))
        return matchStatementInvalid();
    
    if (tokens.at(currentIndex)->getKind() == Token::Kind::NEW_LINE)
        currentIndex++;

    return make_shared<StatementBlock>(statements);
}

shared_ptr<Statement> Parser::matchStatementReturn() {
    if (tokens.at(currentIndex)->getKind() != Token::Kind::RETURN)
        return nullptr;
    
    currentIndex++;

    shared_ptr<Expression> expression = nextExpression();
    if (expression != nullptr && !expression->isValid())
        return matchStatementInvalid();

    if (tokens.at(currentIndex)->getKind() != Token::Kind::NEW_LINE)
        return matchStatementInvalid();
    
    currentIndex++; // new line
    
    return make_shared<StatementReturn>(expression);
}

shared_ptr<Statement> Parser::matchStatementExpression() {
    shared_ptr<Expression> expression = nextExpression();

    if (expression == nullptr)
        return nullptr;
    else if (!expression->isValid())
        return make_shared<StatementInvalid>(tokens.at(currentIndex));

    currentIndex++;
    return make_shared<StatementExpression>(expression);
}

shared_ptr<StatementInvalid> Parser::matchStatementInvalid() {
    return make_shared<StatementInvalid>(tokens.at(currentIndex));
}

//
// Expression
//
shared_ptr<Expression> Parser::nextExpression() {
    return nullptr;
}

/*shared_ptr<Expression> Parser::term() {
    shared_ptr<Expression> expression = factor();
    if (!expression->isValid())
        return expression;

    while (tokens.at(currentIndex)->isOfKind({Token::Kind::PLUS, Token::Kind::MINUS})) {
        expression = matchBinary(expression);
    }

    return expression;
}

shared_ptr<Expression> Parser::factor() {
    shared_ptr<Expression> expression = primary();
    if (!expression->isValid())
        return expression;

    while (tokens.at(currentIndex)->isOfKind({Token::Kind::STAR, Token::Kind::SLASH, Token::Kind::PERCENT})) {
        expression = matchBinary(expression);
    }

    return expression;
}

shared_ptr<Expression> Parser::primary() {
    {
        shared_ptr<Expression> expression = matchInteger();
        if (expression->isValid())
            return expression;
    }

    {
        shared_ptr<Expression> expression = matchGrouping();
        if (expression->isValid())
            return expression;
    }

    return make_shared<Expression>(Expression::Kind::INVALID, tokens.at(currentIndex), nullptr, nullptr);
}

shared_ptr<Expression> Parser::matchInteger() {
    Token token = tokens.at(currentIndex);
    if (token.getKind() == Token::Kind::INTEGER) {
        currentIndex++;
        return make_shared<Expression>(Expression::Kind::LITERAL, token, nullptr, nullptr);
    }

    return make_shared<Expression>(Expression::Kind::INVALID, token, nullptr, nullptr);
}

shared_ptr<Expression> Parser::matchGrouping() {
    Token token = tokens.at(currentIndex);
    if (token.getKind() == Token::Kind::LEFT_PAREN) {
        currentIndex++;
        shared_ptr<Expression> expression = term();
        // has grouped expression failed?
        if (!expression->isValid())
            return expression;
        if (tokens.at(currentIndex)->getKind() == Token::Kind::RIGHT_PAREN) {
            currentIndex++;
            return make_shared<Expression>(Expression::Kind::GROUPING, token, expression, nullptr);
        }
    }

    return make_shared<Expression>(Expression::Kind::INVALID, token, nullptr, nullptr);
}

shared_ptr<Expression> Parser::matchBinary(shared_ptr<Expression> left) {
    Token token = tokens.at(currentIndex);
    if (token.isOfKind({Token::Kind::PLUS, Token::Kind::MINUS, Token::Kind::STAR, Token::Kind::SLASH, Token::Kind::PERCENT})) {
        currentIndex++;
        shared_ptr<Expression> right = factor();
        // Has right expression failed?
        if (!right->isValid())
            return right;
        return make_shared<Expression>(Expression::Kind::BINARY, token, left, right);
    }

    return make_shared<Expression>(Expression::Kind::INVALID, token, nullptr, nullptr);
}*/

bool Parser::matchesTokenKinds(vector<Token::Kind> kinds) {
    if (currentIndex + kinds.size() >= tokens.size())
        return false;

    for (int i=0; i<kinds.size(); i++) {
        if (kinds.at(i) != tokens.at(currentIndex + i)->getKind())
            return false;
    }

    return true;
}
