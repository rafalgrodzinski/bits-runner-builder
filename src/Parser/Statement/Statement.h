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
    LOOP,
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