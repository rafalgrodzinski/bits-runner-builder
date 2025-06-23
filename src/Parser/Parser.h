#ifndef PARSER_H
#define PARSER_H

#include <vector>

#include "Lexer/Token.h"
#include "Expression.h"
#include "Parser/Statement/Statement.h"

class StatementInvalid;

using namespace std;

class Parser {
private:
    vector<shared_ptr<Token>> tokens;
    int currentIndex = 0;

    shared_ptr<Statement> nextStatement();
    shared_ptr<Statement> matchStatementFunctionDeclaration();
    shared_ptr<Statement> matchStatementVarDeclaration();
    shared_ptr<Statement> matchStatementBlock(vector<TokenKind> terminalTokenKinds, bool shouldConsumeTerminal);
    shared_ptr<Statement> matchStatementReturn();
    shared_ptr<Statement> matchStatementExpression();

    shared_ptr<Statement> matchStatementMetaExternFunction();

    shared_ptr<StatementInvalid> matchStatementInvalid(string message = "");

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
    shared_ptr<Expression> matchExpressionVar();
    shared_ptr<Expression> matchExpressionCall();
    shared_ptr<Expression> matchExpressionBlock(vector<TokenKind> terminalTokenKinds, bool shouldConsumeTerminal);
    shared_ptr<ExpressionInvalid> matchExpressionInvalid();

    bool tryMatchingTokenKinds(vector<TokenKind> kinds, bool shouldMatchAll, bool shouldAdvance);
    optional<ValueType> valueTypeForToken(shared_ptr<Token> token);

public:
    Parser(vector<shared_ptr<Token>> tokens);
    vector<shared_ptr<Statement>> getStatements();
};

#endif