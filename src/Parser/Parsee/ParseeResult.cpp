#include "ParseeResult.h"

#include "Lexer/Token.h"
#include "Parser/ValueType.h"

ParseeResult ParseeResult::tokenResult(shared_ptr<Token> token) {
    ParseeResult parseeResult;
    parseeResult.kind = ParseeResultKind::TOKEN;
    parseeResult.token = token;
    parseeResult.tokensCount = 1;
    return parseeResult;
}

ParseeResult ParseeResult::valueTypeResult(shared_ptr<ValueType> valueType, int tokensCount) {
    ParseeResult parseeResult;
    parseeResult.kind = ParseeResultKind::VALUE_TYPE;
    parseeResult.valueType = valueType;
    parseeResult.tokensCount = tokensCount;
    return parseeResult;    
}

ParseeResult ParseeResult::expressionResult(shared_ptr<Expression> expression, int tokensCount) {
    ParseeResult result;
    result.kind = ParseeResultKind::EXPRESSION;
    result.expression = expression;
    result.tokensCount = tokensCount;
    return result;
}

ParseeResult::ParseeResult() { }


ParseeResultKind ParseeResult::getKind() {
    return kind;
}

shared_ptr<Token> ParseeResult::getToken() {
    return token;
}

shared_ptr<ValueType> ParseeResult::getValueType() {
    return valueType;
}

shared_ptr<Expression> ParseeResult::getExpression() {
    return expression;
}

int ParseeResult::getTokensCount() {
    return tokensCount;
}