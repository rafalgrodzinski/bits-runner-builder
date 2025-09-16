#include "ParseeResult.h"

#include "Lexer/Token.h"
#include "Parser/ValueType.h"

ParseeResult ParseeResult::tokenResult(shared_ptr<Token> token, int tag) {
    ParseeResult parseeResult;
    parseeResult.kind = ParseeResultKind::TOKEN;
    parseeResult.tag = tag;
    parseeResult.token = token;
    parseeResult.tokensCount = 1;
    return parseeResult;
}

ParseeResult ParseeResult::valueTypeResult(shared_ptr<ValueType> valueType, int tokensCount, int tag) {
    ParseeResult parseeResult;
    parseeResult.kind = ParseeResultKind::VALUE_TYPE;
    parseeResult.tag = tag;
    parseeResult.valueType = valueType;
    parseeResult.tokensCount = tokensCount;
    return parseeResult;    
}

ParseeResult ParseeResult::statementResult(shared_ptr<Statement> statement, int tokensCount, int tag) {
    ParseeResult parseeResult;
    parseeResult.kind = ParseeResultKind::STATEMENT;
    parseeResult.tag = tag;
    parseeResult.statement = statement;
    parseeResult.tokensCount = tokensCount;
    return parseeResult;
}

ParseeResult ParseeResult::statementInBlockResult(shared_ptr<Statement> statement, int tokensCount, int tag) {
    ParseeResult parseeResult;
    parseeResult.kind = ParseeResultKind::STATEMENT_IN_BLOCK;
    parseeResult.tag = tag;
    parseeResult.statement = statement;
    parseeResult.tokensCount = tokensCount;
    return parseeResult;
}

ParseeResult ParseeResult::expressionResult(shared_ptr<Expression> expression, int tokensCount, int tag) {
    ParseeResult parseeResult;
    parseeResult.kind = ParseeResultKind::EXPRESSION;
    parseeResult.tag = tag;
    parseeResult.expression = expression;
    parseeResult.tokensCount = tokensCount;
    return parseeResult;
}

ParseeResult::ParseeResult() { }


ParseeResultKind ParseeResult::getKind() {
    return kind;
}

int ParseeResult::getTag() {
    return tag;
}

shared_ptr<Token> ParseeResult::getToken() {
    return token;
}

shared_ptr<ValueType> ParseeResult::getValueType() {
    return valueType;
}

shared_ptr<Statement> ParseeResult::getStatement() {
    return statement;
}

shared_ptr<Expression> ParseeResult::getExpression() {
    return expression;
}

int ParseeResult::getTokensCount() {
    return tokensCount;
}