#ifndef STATEMENT_H
#define STATEMENT_H

#include <iostream>

using namespace std;

enum class StatementKind {
    EXPRESSION,
    BLOCK,
    RETURN,
    FUNCTION,
    RAW_FUNCTION,
    VARIABLE,
    ASSIGNMENT,
    REPEAT,
    META_EXTERN_FUNCTION,
    TYPE
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