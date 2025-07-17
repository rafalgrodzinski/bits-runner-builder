#ifndef PARSEE_RESULT_H
#define PARSEE_RESULT_H

#include <memory>

class Token;
class ValueType;

using namespace std;

enum class ParseeResultKind {
    TOKEN,
    VALUE_TYPE,
};

class ParseeResult {
private:
    ParseeResultKind kind;
    shared_ptr<Token> token;
    shared_ptr<ValueType> valueType;
    ParseeResult();

public:
    static ParseeResult tokenResult(shared_ptr<Token> token);
    static ParseeResult valueTypeResult(shared_ptr<ValueType> valueType);

    ParseeResultKind getKind();
    shared_ptr<Token> getToken();
    shared_ptr<ValueType> getValueType();
};

#endif