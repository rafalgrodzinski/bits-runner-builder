#ifndef PARSER_H
#define PARSER_H

#include <vector>

#include "Token.h"
#include "Expression.h"
#include "Statement.h"

using namespace std;

class Parser {
private:
    vector<shared_ptr<Token>> tokens;
    int currentIndex = 0;

    shared_ptr<Statement> nextStatement();
    shared_ptr<Statement> matchStatementFunctionDeclaration();
    shared_ptr<Statement> matchStatementBlock();
    shared_ptr<Statement> matchStatementReturn();
    shared_ptr<Statement> matchStatementExpression();
    shared_ptr<StatementInvalid> matchStatementInvalid();

    shared_ptr<Expression> nextExpression();
    /*shared_ptr<Expression> term(); // +, -
    shared_ptr<Expression> factor(); // *, /, %
    shared_ptr<Expression> primary(); // integer, ()

    shared_ptr<Expression> matchInteger();
    shared_ptr<Expression> matchGrouping();
    shared_ptr<Expression> matchBinary(shared_ptr<Expression> left);*/
    shared_ptr<ExpressionInvalid> matchExpressionInvalid();

    bool matchesTokenKinds(vector<Token::Kind> kinds);

public:
    Parser(vector<shared_ptr<Token>> tokens);
    vector<shared_ptr<Statement>> getStatements();
};

#endif