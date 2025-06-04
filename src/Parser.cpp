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

    return make_shared<StatementExpression>(expression);
}

shared_ptr<StatementInvalid> Parser::matchStatementInvalid() {
    return make_shared<StatementInvalid>(tokens.at(currentIndex));
}

//
// Expression
//
shared_ptr<Expression> Parser::nextExpression() {
    return matchEquality();
}

shared_ptr<Expression> Parser::matchEquality() {
    shared_ptr<Expression> expression = matchTerm();
    if (expression == nullptr || !expression->isValid())
        return expression;

    while (tokens.at(currentIndex)->isOfKind(Token::tokensEquality)) {
        // comaprison
        expression = matchExpressionBinary(expression);
    }

    return expression;
}

shared_ptr<Expression> Parser::matchComparison() {
    shared_ptr<Expression> expression = matchTerm();
    if (expression == nullptr || !expression->isValid())
        return expression;
    
    while (tokens.at(currentIndex)->isOfKind(Token::tokensComparison)) {
        // term
        expression = matchExpressionBinary(expression);
    }
    
    return expression;
}

shared_ptr<Expression> Parser::matchTerm() {
    shared_ptr<Expression> expression = matchFactor();
    if (expression == nullptr || !expression->isValid())
        return expression;

    while (tokens.at(currentIndex)->isOfKind(Token::tokensTerm)) {
        // factor
        expression = matchExpressionBinary(expression);
    }

    return expression;
}

shared_ptr<Expression> Parser::matchFactor() {
    shared_ptr<Expression> expression = matchPrimary();
    if (expression == nullptr || !expression->isValid())
        return expression;

    while (tokens.at(currentIndex)->isOfKind(Token::tokensFactor)) {
        // unary
        expression = matchExpressionBinary(expression);
    }

    return expression;
}

shared_ptr<Expression> Parser::matchPrimary() {
    shared_ptr<Expression> expression;

    expression = matchExpressionLiteral();
    if (expression != nullptr)
        return expression;
    
    expression = matchExpressionGrouping();
    if (expression != nullptr)
        return expression;

    return nullptr;
}

shared_ptr<Expression> Parser::matchExpressionLiteral() {
    shared_ptr<Token> token = tokens.at(currentIndex);
    if (token->isOfKind({Token::Kind::INTEGER})) {
        currentIndex++;
        return make_shared<ExpressionLiteral>(token);
    }

    return nullptr;
}

shared_ptr<Expression> Parser::matchExpressionGrouping() {
    shared_ptr<Token> token = tokens.at(currentIndex);
    if (token->getKind() == Token::Kind::LEFT_PAREN) {
        currentIndex++;
        shared_ptr<Expression> expression = matchTerm();
        // has grouped expression failed?
        if (expression == nullptr) {
            return matchExpressionInvalid();
        } else if(!expression->isValid()) {
            return expression;
        } else if (tokens.at(currentIndex)->getKind() == Token::Kind::RIGHT_PAREN) {
            currentIndex++;
            return make_shared<ExpressionGrouping>(expression);
        } else {
            return matchExpressionInvalid();
        }
    }

    return nullptr;
}

shared_ptr<Expression> Parser::matchExpressionBinary(shared_ptr<Expression> left) {
    shared_ptr<Token> token = tokens.at(currentIndex);
    if (token->isOfKind(Token::tokensBinary)) {
        currentIndex++;
        shared_ptr<Expression> right = matchFactor();
        if (right == nullptr) {
            return matchExpressionInvalid();
        } else if (!right->isValid()) {
            return right;
        } else {
            return make_shared<ExpressionBinary>(token, left, right);
        }
    }

    return nullptr;
}

shared_ptr<ExpressionInvalid> Parser::matchExpressionInvalid() {
    return make_shared<ExpressionInvalid>(tokens.at(currentIndex));
}

bool Parser::matchesTokenKinds(vector<Token::Kind> kinds) {
    if (currentIndex + kinds.size() >= tokens.size())
        return false;

    for (int i=0; i<kinds.size(); i++) {
        if (kinds.at(i) != tokens.at(currentIndex + i)->getKind())
            return false;
    }

    return true;
}
