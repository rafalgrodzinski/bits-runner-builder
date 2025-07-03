#ifndef STATEMENT_H
#define STATEMENT_H

#include <iostream>

#include "Types.h"

using namespace std;

enum class StatementKind {
    EXPRESSION,
    BLOCK,
    RETURN,
    FUNCTION,
    VARIABLE,
    ASSIGNMENT,
    REPEAT,
    META_EXTERN_FUNCTION
};

class Statement {
private:
    StatementKind kind;

public:
    Statement(StatementKind kind);
    virtual ~Statement() { }
    StatementKind getKind();
};

#endif