#include "Parser.h"

Parser::Parser(vector<Token> tokens): tokens(tokens) {
}

/*shared_ptr<Expression> Parser::getExpression() {
    shared_ptr<Expression> expression = term();
    if (!expression->isValid()) {
        cerr << "Unexpected token '" << expression->getToken().getLexme() << "' at " << expression->getToken().getLine() << ":" << expression->getToken().getColumn() << endl;
        return nullptr;
    }
    return expression;
}*/

vector<shared_ptr<Statement>> Parser::getStatements() {
    vector<shared_ptr<Statement>> statements;
        statements.push_back(make_shared<Statement>(Statement::Kind::INVALID));
    return statements;
}

shared_ptr<Expression> Parser::term() {
    shared_ptr<Expression> expression = factor();

    while (tokens.at(currentIndex).isOfKind({Token::Kind::PLUS, Token::Kind::MINUS})) {
        expression = matchBinary(expression);
    }

    return expression;
}

shared_ptr<Expression> Parser::factor() {
    shared_ptr<Expression> expression = primary();

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