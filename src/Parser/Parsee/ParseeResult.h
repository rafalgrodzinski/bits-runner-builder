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
    STATEMENT_IN_BLOCK,
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
    static ParseeResult statementInBlockResult(shared_ptr<Statement> statement, int tokensCount, int tag = -1);
    static ParseeResult expressionResult(shared_ptr<Expression> expression, int tokensCount, int tag = -1);

    ParseeResultKind getKind() const;
    int getTag() const;
    shared_ptr<Token> getToken() const;
    shared_ptr<ValueType> getValueType() const;
    shared_ptr<Statement> getStatement() const;
    shared_ptr<Expression> getExpression() const;
    int getTokensCount() const;
};

#endif