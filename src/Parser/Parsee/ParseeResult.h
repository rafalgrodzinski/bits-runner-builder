#ifndef PARSEE_RESULT_H
#define PARSEE_RESULT_H

#include <memory>

class Token;
class ValueType;
class Expression;

using namespace std;

enum class ParseeResultKind {
    TOKEN,
    VALUE_TYPE,
    EXPRESSION
};

class ParseeResult {
private:
    ParseeResultKind kind;
    shared_ptr<Token> token;
    shared_ptr<ValueType> valueType;
    shared_ptr<Expression> expression;
    int tokensCount;
    ParseeResult();

public:
    static ParseeResult tokenResult(shared_ptr<Token> token);
    static ParseeResult valueTypeResult(shared_ptr<ValueType> valueType, int tokensCount);
    static ParseeResult expressionResult(shared_ptr<Expression> expression, int tokensCount);

    ParseeResultKind getKind();
    shared_ptr<Token> getToken();
    shared_ptr<ValueType> getValueType();
    shared_ptr<Expression> getExpression();
    int getTokensCount();
};

#endif