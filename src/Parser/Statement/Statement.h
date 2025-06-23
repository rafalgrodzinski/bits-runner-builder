#ifndef STATEMENT_H
#define STATEMENT_H

#include <iostream>

#include "Lexer/Token.h"
#include "Parser/Expression.h"
#include "Types.h"

class Expression;
class Statement;
class StatementBlock;
class StatementReturn;
class StatementExpression;
class StatementInvalid;

using namespace std;

enum class StatementKind {
    FUNCTION_DECLARATION,
    VAR_DECLARATION,
    BLOCK,
    RETURN,
    EXPRESSION,
    META_EXTERN_FUNCTION,
    INVALID
};

class Statement {
private:
    StatementKind kind;

public:
    Statement(StatementKind kind);
    StatementKind getKind();
    bool isValid();
    virtual string toString(int indent);
};

#endif