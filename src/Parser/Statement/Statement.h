#ifndef STATEMENT_H
#define STATEMENT_H

#include <iostream>

using namespace std;

enum class StatementKind {
    ASSIGNMENT,
    BLOB, // !
    BLOB_DECLARATION, // !
    BLOCK,
    EXPRESSION,
    FUNCTION,
    FUNCTION_DECLARATION, // !
    META_EXTERN_VARIABLE, // !
    META_EXTERN_FUNCTION, // !
    META_IMPORT, // !
    MODULE,
    RAW_FUNCTION, // !
    REPEAT,
    RETURN,
    VARIABLE_DECLARATION, // !
    VARIABLE
};

class Statement {
private:
    StatementKind kind;
    int line;
    int column;

public:
    Statement(StatementKind kind, int line, int column);
    virtual ~Statement() { }
    StatementKind getKind();
    int getLine();
    int getColumn();
};

#endif