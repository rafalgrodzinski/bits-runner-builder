#ifndef PARSEE_RESULT_H
#define PARSEE_RESULT_H

#include <memory>

class Token;
class ValueType;
class Statement;
class Expression;

using namespace std;

enum class ParseeResultKind {
    TOKEN,
    VALUE_TYPE,
    STATEMENT,
    EXPRESSION
};

class ParseeResult {
private:
    ParseeResultKind kind;
    int tag;
    shared_ptr<Token> token;
    shared_ptr<ValueType> valueType;
    shared_ptr<Statement> statement;
    shared_ptr<Expression> expression;
    int tokensCount;
    ParseeResult();

public:
    static ParseeResult tokenResult(shared_ptr<Token> token, int tag = -1);
    static ParseeResult valueTypeResult(shared_ptr<ValueType> valueType, int tokensCount, int tag = -1);
    static ParseeResult statementResult(shared_ptr<Statement> statement, int tokensCount, int tag = -1);
    static ParseeResult expressionResult(shared_ptr<Expression> expression, int tokensCount, int tag = -1);

    ParseeResultKind getKind();
    int getTag();
    shared_ptr<Token> getToken();
    shared_ptr<ValueType> getValueType();
    shared_ptr<Statement> getStatement();
    shared_ptr<Expression> getExpression();
    int getTokensCount();
};

#endif