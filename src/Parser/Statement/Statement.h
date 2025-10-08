#ifndef STATEMENT_H
#define STATEMENT_H

#include <iostream>

using namespace std;

enum class StatementKind {
    MODULE,
    EXPRESSION,
    BLOCK,
    RETURN,
    FUNCTION_DECLARATION,
    FUNCTION,
    RAW_FUNCTION,
    VARIABLE_DECLARATION,
    VARIABLE,
    ASSIGNMENT,
    REPEAT,
    META_EXTERN_FUNCTION,
    META_IMPORT,
    BLOB_DECLARATION,
    BLOB
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