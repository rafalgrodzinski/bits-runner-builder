#ifndef PARSEE_RESULT_H
#define PARSEE_RESULT_H

#include <memory>

class Expression;
class Pattern;
class Statement;
class Token;
class ValueType;

enum class ParseeResultKind {
    EXPRESSION,
    PATTERN,
    STATEMENT,
    STATEMENT_IN_BLOCK,
    TOKEN,
    VALUE_TYPE
};

class ParseeResult {
public:
    static ParseeResult tokenResult(std::shared_ptr<Token> token, int tag = -1);
    static ParseeResult valueTypeResult(std::shared_ptr<ValueType> valueType, int tokensCount, int tag = -1);
    static ParseeResult statementResult(std::shared_ptr<Statement> statement, int tokensCount, int tag = -1);
    static ParseeResult statementInBlockResult(std::shared_ptr<Statement> statement, int tokensCount, int tag = -1);
    static ParseeResult expressionResult(std::shared_ptr<Expression> expression, int tokensCount, int tag = -1);
    static ParseeResult patternResult(std::shared_ptr<Pattern> pattern, int tokensCount, int tag = -1);

    ParseeResultKind getKind() const;
    int getTag() const;
    std::shared_ptr<Token> getToken() const;
    std::shared_ptr<ValueType> getValueType() const;
    std::shared_ptr<Statement> getStatement() const;
    std::shared_ptr<Expression> getExpression() const;
    std::shared_ptr<Pattern> getPattern() const;
    int getTokensCount() const;

private:
    ParseeResultKind kind;
    int tag;
    std::shared_ptr<Token> token;
    std::shared_ptr<ValueType> valueType;
    std::shared_ptr<Statement> statement;
    std::shared_ptr<Expression> expression;
    std::shared_ptr<Pattern> pattern;
    int tokensCount;
    ParseeResult();
};

#endif