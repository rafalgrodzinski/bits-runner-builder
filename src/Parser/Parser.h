#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include "Types.h"

class Token;
enum class TokenKind;
class Error;

class Expression;
class Statement;

using namespace std;

class Parser {
private:
    vector<shared_ptr<Token>> tokens;
    int currentIndex = 0;
    vector<shared_ptr<Error>> errors;

    shared_ptr<Statement> nextStatement();
    shared_ptr<Statement> nextInBlockStatement();

    shared_ptr<Statement> matchStatementMetaExternFunction();
    shared_ptr<Statement> matchStatementVariable();
    shared_ptr<Statement> matchStatementFunction();

    shared_ptr<Statement> matchStatementBlock(vector<TokenKind> terminalTokenKinds);
    shared_ptr<Statement> matchStatementAssignment();
    shared_ptr<Statement> matchStatementReturn();
    shared_ptr<Statement> matchStatementRepeat();
    shared_ptr<Statement> matchStatementExpression();

    shared_ptr<Expression> nextExpression();
    shared_ptr<Expression> matchEquality(); // =, !=
    shared_ptr<Expression> matchComparison(); // <, <=, >, >=
    shared_ptr<Expression> matchTerm(); // +, -
    shared_ptr<Expression> matchFactor(); // *, /, %
    shared_ptr<Expression> matchPrimary(); // integer, ()

    shared_ptr<Expression> matchExpressionGrouping();
    shared_ptr<Expression> matchExpressionLiteral();
    shared_ptr<Expression> matchExpressionVariable();
    shared_ptr<Expression> matchExpressionCall();
    shared_ptr<Expression> matchExpressionIfElse();
    shared_ptr<Expression> matchExpressionBinary(shared_ptr<Expression> left);
    shared_ptr<Expression> matchExpressionBlock(vector<TokenKind> terminalTokenKinds);

    bool tryMatchingTokenKinds(vector<TokenKind> kinds, bool shouldMatchAll, bool shouldAdvance);
    optional<ValueType> valueTypeForToken(shared_ptr<Token> token);

    void markError(optional<TokenKind> expectedTokenKind, optional<string> message);

public:
    Parser(vector<shared_ptr<Token>> tokens);
    vector<shared_ptr<Statement>> getStatements();
};

#endif