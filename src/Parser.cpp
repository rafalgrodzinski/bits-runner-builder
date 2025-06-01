#include "Parser.h"

Parser::Parser(vector<Token> tokens): tokens(tokens) {
}

vector<shared_ptr<Statement>> Parser::getStatements() {
    vector<shared_ptr<Statement>> statements;

    while (tokens.at(currentIndex).getKind() != Token::Kind::END) {
        shared_ptr<Statement> statement = nextStatement();
        // Abort parsing if we got an error
        if (!statement->isValid()) {
            cerr << "Unexpected token '" << statement->getToken()->getLexme() << "' at " << statement->getToken()->getLine() << ":" << statement->getToken()->getColumn() << endl;
            return vector<shared_ptr<Statement>>();
        }
        statements.push_back(statement);
    }

    return statements;
}

//
// Statement
//
shared_ptr<Statement> Parser::nextStatement() {
    {
        shared_ptr<Statement> statement = matchFunctionDeclarationStatement();
        if (statement->isValid())
            return statement;
    }

    {
        shared_ptr<Statement> statement = matchExpressionStatement();
        if (statement->isValid())
            return statement;
    }

    return matchInvalidStatement();
}

shared_ptr<Statement> Parser::matchFunctionDeclarationStatement() {
    if (!matchesTokenKinds({Token::Kind::IDENTIFIER, Token::Kind::COLON, Token::Kind::FUNCTION}))
        return make_shared<Statement>(Statement::Kind::INVALID, make_shared<Token>(tokens.at(currentIndex)), nullptr, nullptr, vector<shared_ptr<Statement>>(), "");

    Token identifierToken = tokens.at(currentIndex);
    currentIndex++;
    currentIndex++; // skip colon
    currentIndex++; // skip fun

    while (tokens.at(currentIndex).getKind() != Token::Kind::NEW_LINE) {
        currentIndex++;
    }
    currentIndex++; // new line
    shared_ptr<Statement> blockStatement = matchBlockStatement();
    if (!blockStatement->isValid())
        return blockStatement;
    
    return make_shared<Statement>(Statement::Kind::FUNCTION_DECLARATION, nullptr, nullptr, blockStatement, vector<shared_ptr<Statement>>(), identifierToken.getLexme());
}

shared_ptr<Statement> Parser::matchBlockStatement() {
    vector<shared_ptr<Statement>> statements;

    while (tokens.at(currentIndex).getKind() != Token::Kind::SEMICOLON) {
        shared_ptr<Statement> statement = nextStatement();
        if (!statement->isValid())
            return statement;
        statements.push_back(statement);
    }
    currentIndex++; // skip ;

    if (!tokens.at(currentIndex).isOfKind({Token::Kind::NEW_LINE, Token::Kind::END}))
        return make_shared<Statement>(Statement::Kind::INVALID, make_shared<Token>(tokens.at(currentIndex)), nullptr, nullptr, vector<shared_ptr<Statement>>(), "");
    
    if (tokens.at(currentIndex).getKind() == Token::Kind::NEW_LINE)
        currentIndex++;

    return make_shared<Statement>(Statement::Kind::BLOCK, nullptr, nullptr, nullptr, statements, "");
}

shared_ptr<Statement> Parser::matchInvalidStatement() {
    return make_shared<Statement>(Statement::Kind::INVALID, make_shared<Token>(tokens.at(currentIndex)), nullptr, nullptr, vector<shared_ptr<Statement>>(), "");
}

//
// Expression
//
shared_ptr<Statement> Parser::matchExpressionStatement() {
    shared_ptr<Expression> expression = term();
    if (expression->isValid() && tokens.at(currentIndex).isOfKind({Token::Kind::NEW_LINE, Token::Kind::END})) {
        currentIndex++;
        return make_shared<Statement>(Statement::Kind::EXPRESSION, nullptr, expression, nullptr, vector<shared_ptr<Statement>>(), "");
    } else {
        return make_shared<Statement>(Statement::Kind::INVALID, make_shared<Token>(tokens.at(currentIndex)), expression, nullptr, vector<shared_ptr<Statement>>(), "");
    }
}

shared_ptr<Expression> Parser::term() {
    shared_ptr<Expression> expression = factor();
    if (!expression->isValid())
        return expression;

    while (tokens.at(currentIndex).isOfKind({Token::Kind::PLUS, Token::Kind::MINUS})) {
        expression = matchBinary(expression);
    }

    return expression;
}

shared_ptr<Expression> Parser::factor() {
    shared_ptr<Expression> expression = primary();
    if (!expression->isValid())
        return expression;

    while (tokens.at(currentIndex).isOfKind({Token::Kind::STAR, Token::Kind::SLASH, Token::Kind::PERCENT})) {
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
        if (tokens.at(currentIndex).getKind() == Token::Kind::RIGHT_PAREN) {
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
}

bool Parser::matchesTokenKinds(vector<Token::Kind> kinds) {
    if (currentIndex + kinds.size() >= tokens.size())
        return false;

    for (int i=0; i<kinds.size(); i++) {
        if (kinds.at(i) != tokens.at(currentIndex + i).getKind())
            return false;
    }
    return true;
}
