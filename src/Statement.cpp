#include "Statement.h"

Statement::Statement(Kind kind, shared_ptr<Token> token, shared_ptr<Expression> expression, shared_ptr<Statement> blockStatement, vector<shared_ptr<Statement>> statements, string name):
    kind(kind), token(token), expression(expression), blockStatement(blockStatement), statements(statements), name(name) {
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
        case BLOCK: {
            string value;
            for (int i=0; i<statements.size(); i++) {
                value += statements.at(i)->toString();
                if (i < statements.size() - 1)
                    value += "\n";
            }
            return value;
        }
        case FUNCTION_DECLARATION:
            return "FUNCTION " + name + "\n" + blockStatement->toString() + "\n;";
        case INVALID:
            return "INVALID";
    }
}