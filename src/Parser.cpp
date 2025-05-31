#include "Parser.h"

Parser::Parser(vector<Token> tokens): tokens(tokens) {
}

shared_ptr<Expression> Parser::getExpression() {
    return term();
}

shared_ptr<Expression> Parser::term() {
    shared_ptr<Expression> expression = factor();

    while (tokens.at(currentIndex).isOneOf({Token::Kind::PLUS, Token::Kind::MINUS})) {
        expression = matchBinary(expression);
    }

    return expression;
}

shared_ptr<Expression> Parser::factor() {
    shared_ptr<Expression> expression = primary();

    while (tokens.at(currentIndex).isOneOf({Token::Kind::STAR, Token::Kind::SLASH, Token::Kind::PERCENT})) {
        expression = matchBinary(expression);
    }

    return expression;
}

shared_ptr<Expression> Parser::primary() {
    shared_ptr<Expression> expression = Expression::Invalid;

    do {
        if((expression = matchInteger()) != Expression::Invalid)
            break;
        
        if((expression = matchGrouping()) != Expression::Invalid)
            break;
    } while(false);

    return expression;
}

shared_ptr<Expression> Parser::matchInteger() {
    Token token = tokens.at(currentIndex);
    if (token.getKind() == Token::Kind::INTEGER) {
        currentIndex++;
        return make_shared<Expression>(Expression::Kind::LITERAL, token, nullptr, nullptr);
    }

    return Expression::Invalid;
}

shared_ptr<Expression> Parser::matchGrouping() {
    Token token = tokens.at(currentIndex);
    if (token.getKind() == Token::Kind::LEFT_PAREN) {
        currentIndex++;
        shared_ptr<Expression> expression = term();
        if (tokens.at(currentIndex).getKind() == Token::Kind::RIGHT_PAREN) {
            currentIndex++;
            return make_shared<Expression>(Expression::Kind::GROUPING, token, expression, nullptr);
        }
    }

    return Expression::Invalid;
}

shared_ptr<Expression> Parser::matchBinary(shared_ptr<Expression> left) {
    Token token = tokens.at(currentIndex);
    if (token.isOneOf({Token::Kind::PLUS, Token::Kind::MINUS, Token::Kind::STAR, Token::Kind::SLASH, Token::Kind::PERCENT})) {
        currentIndex++;
        shared_ptr<Expression> right = factor();
        return make_shared<Expression>(Expression::Kind::BINARY, token, left, right);
    }

    return Expression::Invalid;
}