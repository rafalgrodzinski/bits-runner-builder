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
    shared_ptr<Statement> matchStatementVarDeclaration();
    shared_ptr<Statement> matchStatementBlock();
    shared_ptr<Statement> matchStatementReturn();
    shared_ptr<Statement> matchStatementExpression();
    shared_ptr<StatementInvalid> matchStatementInvalid();

    shared_ptr<Expression> nextExpression();
    shared_ptr<Expression> matchEquality(); // =, !=
    shared_ptr<Expression> matchComparison(); // <, <=, >, >=
    shared_ptr<Expression> matchTerm(); // +, -
    shared_ptr<Expression> matchFactor(); // *, /, %
    shared_ptr<Expression> matchPrimary(); // integer, ()

    shared_ptr<Expression> matchExpressionLiteral();
    shared_ptr<Expression> matchExpressionGrouping();
    shared_ptr<Expression> matchExpressionBinary(shared_ptr<Expression> left);
    shared_ptr<Expression> matchExpressionIfElse();
    shared_ptr<ExpressionInvalid> matchExpressionInvalid();

    bool matchesTokenKinds(vector<TokenKind> kinds);

public:
    Parser(vector<shared_ptr<Token>> tokens);
    vector<shared_ptr<Statement>> getStatements();
};

#endif