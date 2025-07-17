#include "ParseeResult.h"

#include "Lexer/Token.h"
#include "Parser/ValueType.h"

ParseeResult ParseeResult::tokenResult(shared_ptr<Token> token) {
    ParseeResult parseeResult;
    parseeResult.kind = ParseeResultKind::TOKEN;
    parseeResult.token = token;
    return parseeResult;
}

ParseeResult ParseeResult::valueTypeResult(shared_ptr<ValueType> valueType) {
    ParseeResult parseeResult;
    parseeResult.kind = ParseeResultKind::VALUE_TYPE;
    parseeResult.valueType = valueType;
    return parseeResult;    
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