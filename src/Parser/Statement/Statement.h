#ifndef STATEMENT_H
#define STATEMENT_H

#include <iostream>
#include <memory>
#include <string>
#include <vector>

class Location;

using namespace std;

enum class StatementKind {
    ASSIGNMENT,
    BLOB,
    BLOB_DECLARATION,
    BLOCK,
    EXPRESSION,
    FUNCTION,
    FUNCTION_DECLARATION,
    META_EXTERN_FUNCTION,
    META_EXTERN_VARIABLE,
    META_IMPORT,
    MODULE,
    PROTO,
    RAW_FUNCTION,
    REPEAT,
    RETURN,
    VARIABLE,
    VARIABLE_DECLARATION
};

class Statement {
private:
    StatementKind kind;
    shared_ptr<Location> location;

public:
    Statement(StatementKind kind, shared_ptr<Location> location);
    virtual ~Statement() { }
    StatementKind getKind();
    shared_ptr<Location> getLocation();
};

#endif