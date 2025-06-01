#include "Statement.h"

Statement::Statement(Kind kind, shared_ptr<Token> token, shared_ptr<Expression> expression): kind(kind), token(token), expression(expression) {
}

shared_ptr<Expression> Statement::getExpression() {
    return expression;
}

shared_ptr<Token> Statement::getToken() {
    return token;
}

bool Statement::isValid() {
    return kind != Statement::Kind::INVALID;
}

string Statement::toString() {
    switch (kind) {
        case EXPRESSION:
            return expression->toString();
        case INVALID:
            return "INVALID";
    }
}